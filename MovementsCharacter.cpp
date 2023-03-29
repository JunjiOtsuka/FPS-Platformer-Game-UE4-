// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MovementsCharacter.h"
#include "MovementsProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "DrawDebugHelpers.h"
// #include "SwingDoor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "CableComponent.h"
#include "Components/InterpToMovementComponent.h"
#include "WallRunChecker.h"
#include "GameFramework/Actor.h"
// #include "GrapplingHook.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMovementsCharacter

AMovementsCharacter::AMovementsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	// create trigger capsule
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->InitCapsuleSize(55.f, 96.0f);;
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(RootComponent);

	// bind trigger events
	// TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMovementsCharacter::OnOverlapBegin);
	// TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &AMovementsCharacter::OnOverlapEnd);

	MyActorComponent = this->FindComponentByClass<UCharacterMovementComponent>();

	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("CableDetector"));
	Cable->SetupAttachment(FP_MuzzleLocation);
	// Cable->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	// Cable->SetAttachEndTo(this, RootComponent->GetDefaultSceneRootVariableName());

	WallDetector = CreateDefaultSubobject<UWallRunChecker>(TEXT("WallDetector"));
	// GrapplingHook = CreateDefaultSubobject<UGrapplingHook>(TEXT("WallDetector"));
	SavedVelocity = FVector::ZeroVector;

	//run bool
	isRunning = false;

	//wall actions
	bWallStick = false;
	MaxFuzeTime = 1.0f;
	bTimerStarted = false;
	bForwardInput = false;
	bBackInput = false;
	bRightInput = false;
	bLeftInput = false;

	//wall run
	IsWallRunning = false;

	//wall run related force
	NormalForce = 4.0f;
	ForwardForce = 1.5f;
	UpForce = 4.0f;
	ForceMultiplier = 150.f;

	//walljump bool 
	CanWallJump = false;

	//Camera Lock-on 
	CanLockOn = false;

	//grappling bool
	CanGrapple = false;
	IsGrappling = false;
	EndTarget;
	TimeElapsed = 0.0f;
	LerpDuration = 3.0f;
	UpdatedEndTarget = false;

	//normal vector
	Force = 100.0f;

	//reset vector
	VectorZero = FVector(0.0f, 0.0f, 0.0f);

	//TSharedRef<UTactical> TacticalClassGenerated(new UTactical());
	//m_TacticalDelegate = NULL;
	m_dInterface = TArray<TScriptInterface<ITactical>>{};
	m_Tactical_Type_1 = CreateDefaultSubobject<ATactical1>(TEXT("Tactical1"));
	m_Tactical_Type_2 = CreateDefaultSubobject<ATactical2>(TEXT("Tactical2"));
}

void AMovementsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsWallRunning && !WallDetector->CanWallRunBool)
	{
		OnWallJump();
	}

	if (!WallDetector->CanWallRunBool) {
		StopWallRun();
	}

	if (WallDetector->frontWall) {
		StartWallAction();
	}

	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::F))
	{
		SavedHitPoint = StartCameraLockOn();
		//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Camera locked")));
	}

	if (IsCamLocked)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Camera locked")));
		FirstPersonCameraComponent->AddRelativeRotation(UKismetMathLibrary::FindLookAtRotation(FirstPersonCameraComponent->GetRelativeLocation(), SavedHitPoint));
	}

	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		if (m_dInterface.Num() == 0) return;
		m_dInterface[0]->DoTactical_Implementation();
	}

	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Three))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Button 3 Pressed")));
		bCheckDelegateSubscription();
		m_dInterface.Add(m_Tactical_Type_1);
	}

	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::Five))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Button 5 Pressed")));
		bCheckDelegateSubscription();
		m_dInterface.Add(m_Tactical_Type_2);
	}

	//player (forward input to wall)
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::W))
	{
		bForwardInput = true;
	}
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::W))
	{
		bForwardInput = false;
	}


	//player (away input to wall)
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::S))
	{
		bBackInput = true;
	}
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::S))
	{
		bBackInput = false;
	}

	//player (side input to wall)
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::A))
	{
		bLeftInput = true;
	}
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::A))
	{
		bLeftInput = false;
	}
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::D))
	{
		bRightInput = true;
	}
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::D))
	{
		bRightInput = false;
	}

	/***Grappling Hook***/
	if (IsGrappling)
	{
		//MovementsCharacter targetObject = FindObject<MovementsCharacter>(GetWorld(), "FirstPersonCharacter2");
		MyActorComponent->GravityScale = 0;
		Jump();
		FVector TargetLocation = FVector(EndTarget.ImpactPoint.X, EndTarget.ImpactPoint.Y, EndTarget.ImpactPoint.Z + 100.0f);
		this->SetActorLocation(FMath::VInterpTo(this->GetActorLocation(), TargetLocation, DeltaTime, 3.0f));

		/***End Grappling Hook***/
		if (FVector2D(MyActorComponent->GetActorLocation().X, MyActorComponent->GetActorLocation().Y)
			.Equals(FVector2D(EndTarget.ImpactPoint.X, EndTarget.ImpactPoint.Y), 
			200.0f))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("destination reached")));
			MyActorComponent->GravityScale = 1.0f;
			MyActorComponent->Velocity = VectorZero;
			StopGrappling();
		}
	} 
}

void AMovementsCharacter::bCheckDelegateSubscription()
{
	if (m_dInterface.Num() > 0)
	{
		m_dInterface.Pop();
	}
}

void AMovementsCharacter::StartWallRun()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Gravity Set To 0")));
	MyActorComponent->GravityScale = 0;
	MyActorComponent->SetPlaneConstraintEnabled(true);
	MyActorComponent->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f));
	CanWallJump = true;
	IsWallRunning = true;
	// MyActorComponent->MaxWalkSpeed = 1200.0f; //needs to be a float
	MyActorComponent->Velocity = VectorZero;
	MyActorComponent->AddImpulse((FirstPersonCameraComponent->GetForwardVector()) * 700.0f, true);
	MyActorComponent->AddImpulse(WallDetector->NormalVector * -100.0f, true);
}

void AMovementsCharacter::StopWallRun()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Gravity Set To 1")));
	MyActorComponent->GravityScale = 1;
	MyActorComponent->SetPlaneConstraintEnabled(false);
	CanWallJump = false;
	IsWallRunning = false;
}

void AMovementsCharacter::StartWallAction()
{
	/********Wall actions**********/

	/*
	var target = WallDetection.frontWallhit.transform.position - transform.position;
	//get the angle between rigidbody velocity and negative hit normal
	var angle = Vector3.Angle(target, transform.forward);
	//player looking away from the wall
	if (angle >= 90) return;
	*/

	/*
	//Player grounded
	if (!WallDetector->aboveThreshold)
	{
		bTimerOver = false;
		bTimerStarted = false;
		GetWorld()->GetTimerManager().ClearTimer(FuzeTimerHandle);

		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Reset")));
		//reset conditions here
		//bWallBounce = false;
		bWallStick = false;
		//bWallMoveInput = false;

		return;
	}
	
	//Player above threshold
	if (!WallDetector->frontWall)
	{
		if (WallDetector->aboveThreshold)
		{
			GetCharacterMovement()->GravityScale = 1; 
			//bWallBounce = false;
			bWallStick = false;
			//bWallMoveInput = false;
			GetWorld()->GetTimerManager().ClearTimer(FuzeTimerHandle);
			return;
		}
	}

	//condition when player is climbing and have their back to the wall
	if (!WallDetector->backWall)
	{
		if (WallDetector->aboveThreshold && !IsWallRunning)
		{
			GetCharacterMovement()->GravityScale = 1;
			//bWallBounce = false;
			bWallStick = false;
			//bWallMoveInput = false;
			GetWorld()->GetTimerManager().ClearTimer(FuzeTimerHandle);
			return;
		}
	}
	*/

	if (!bWallStick)
	{
		GetCharacterMovement()->GravityScale = 1; 

		/*
		_rb.useGravity = true;

		//conditions to wall stick

		//when player is coming in at a certain velocity towards wall
		if (VelocityTowardsWall < 90 && !m_CD.bCDEnd())
		{
			SavedVelocity = _rb.velocity;
			bWallStick = true;
		}
		*/

		//when player input forward action
		if (bForwardInput && !bTimerOver)
		{
			bWallStick = true;
		}
		SavedVelocity = GetCharacterMovement()->Velocity;
		bWallStick = true;
		/*
		if (movement.ReadValue<Vector2>().y > 0 && !m_CD.bCDEnd())
		{
			bWallStick = true;
		}
		*/
	}
	else 
	{
		GetCharacterMovement()->GravityScale = 0; //off
		GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);

		MyActorComponent->AddImpulse((WallDetector->NormalVector * -10.0f) * ForceMultiplier, true);

		//player side input to wall
		if (!bRightInput && !bLeftInput && !bForwardInput && !bBackInput)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("stick: no input")));
		}

		if (bRightInput)
		{
			MyActorComponent->AddImpulse((GetOwner()->GetActorRightVector() * 1.0f) * ForceMultiplier, true);
		}

		if (bLeftInput)
		{
			MyActorComponent->AddImpulse((GetOwner()->GetActorRightVector() * -1.0f) * ForceMultiplier, true);
		}

		/*
		if (movement.ReadValue<Vector2>().x != 0)
		{
			//set and start cooldown when player sticks to wall
			m_CD.SetCDTimer(2.5f);
			m_CD.StartCDTimer(m_CD);

			if (m_CD.bCDEnd())
			{
				cc.material = slippery;
				bWallStick = false;
				_rb.useGravity = true;
				m_CD.EndCDTimer();
			}
			else
			{
				//change physics material
				cc.material = maxFriction;
				Debug.Log("Move to side");
				walkSpeed = WallSideWalkSpeed;
				bWallMoveInput = true;
			}
		}
		*/

		if (bForwardInput)
		{
			if (!bTimerStarted)
			{
				//start timer
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("start timer")));
				GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &AMovementsCharacter::OnTimerOver, MaxFuzeTime, false);
				bTimerStarted = true;
			}

			//timer in progress
			if (!bTimerOver)
			{
				MyActorComponent->AddImpulse((GetOwner()->GetActorUpVector() * UpForce) * ForceMultiplier, true);
			}

			//timer is over
			if (bTimerOver)
			{
				Jump();
				GetCharacterMovement()->GravityScale = 1; //on
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("timer over")));
			}
		}
		
		/*
		if (movement.ReadValue<Vector2>().y > 0)
		{
			//set and start cooldown when player sticks to wall
			m_CD.SetCDTimer(2.5f);
			m_CD.StartCDTimer(m_CD);

			if (m_CD.bCDEnd())
			{
				DoJump();

				cc.material = slippery;
				bWallStick = false;
				_rb.useGravity = true;
				m_CD.EndCDTimer();
			}
			else
			{
				//change physics material
				cc.material = maxFriction;
				Debug.Log("climb");
				_rb.AddForce(transform.up * wallClimbSpeed);
				bWallMoveInput = true;
			}
		}
		*/

		if (bBackInput)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("stick: fall")));
			//MyActorComponent->AddImpulse((GetOwner()->GetActorUpVector() * UpForce) * ForceMultiplier, true);
			GetCharacterMovement()->GravityScale = 1; //gravity on
			bWallStick = false;
		}

		/*
		if (movement.ReadValue<Vector2>().y < 0)
		{
			bWallStick = false;
			_rb.useGravity = true;
			bWallMoveInput = true;
			cc.material = slippery;
		}
		*/

		/*
		if (bWallMoveInput)
		{
			if (m_CD.bCDEnd())
			{
				cc.material = slippery;
				bWallStick = false;
				_rb.useGravity = true;
				m_CD.EndCDTimer();
			}
			else
			{
				//change physics material
				cc.material = maxFriction;
			}
		}
		*/

		if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("stick: jump input")));

			GetCharacterMovement()->GravityScale = 1; //gravity on
			bWallStick = false;

			//_rb.velocity = Vector3.Reflect(SavedVelocity, WallDetection.frontWallhit.normal);

			GetCharacterMovement()->Velocity = FMath::GetReflectionVector(SavedVelocity, WallDetector->forwardNormal);

			//Do jump action
			Jump();

			//bWallBounce = true;
			//reset saved velocity
			SavedVelocity = FVector::ZeroVector;
		}

		/*
		if (!bWallMoveInput) {
			//player jump input
			if (jumpAction.WasPerformedThisFrame() && !bWallMoveInput && !bWallBounce)
			{
				_rb.useGravity = true;
				bWallStick = false;

				//reset physics material
				cc.material = slippery;

				//reflect and apply new velocity
				_rb.velocity = Vector3.Reflect(SavedVelocity, WallDetection.frontWallhit.normal);
				//Do jump action
				DoJump();

				bWallBounce = true;
				//reset saved velocity
				SavedVelocity = Vector3.zero;
			}
		}
		*/

		/*
		if (!bTimerStarted)
		{
			//start timer
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("start timer")));
			GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &AMovementsCharacter::OnTimerOver, MaxFuzeTime, false);
			bTimerStarted = true;
		}

		//timer in progress
		if (!bTimerOver)
		{
			GetCharacterMovement()->GravityScale = 0; //off
			GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);

			if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("wall bounce")));
			}
		}

		//timer is over
		if (bTimerOver)
		{
			GetCharacterMovement()->GravityScale = 1; //on
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("timer over")));
		}
		*/
	}
}

void AMovementsCharacter::StopWallAction()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Gravity Set To 1")));
}

void AMovementsCharacter::OnTimerOver()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("btimerover")));
	bTimerOver = true;
}

void AMovementsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMovementsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind wall jump
	PlayerInputComponent->BindAction("WallRun", IE_Pressed, this, &AMovementsCharacter::OnWallRun);

	// Bind Grappling
	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AMovementsCharacter::StartGrappling);

	// Bind Run
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMovementsCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMovementsCharacter::StopRun);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMovementsCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMovementsCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMovementsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMovementsCharacter::MoveRight);

	

	// Bind quit events
	PlayerInputComponent->BindAction("Quit", IE_Pressed, this, &AMovementsCharacter::Quit);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMovementsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMovementsCharacter::LookUpAtRate);
}

void AMovementsCharacter::Quit()
{
	UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
}

void AMovementsCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AMovementsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMovementsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AMovementsCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMovementsCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMovementsCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AMovementsCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}


void AMovementsCharacter::StartGrappling()
{
	FVector GunStart = FP_Gun->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector ForwardEnd = (GunStart + (ForwardVector * 5000.0f));
	FHitResult forwardOutHit;
	FCollisionQueryParams CollisionParams;

	CanGrapple = GetWorld()->LineTraceSingleByChannel(forwardOutHit, GunStart, ForwardEnd, ECC_Visibility, CollisionParams);
	DrawDebugLine(GetWorld(), GunStart, ForwardEnd, FColor::Green, false, 1, 0, 1);

	if (CanGrapple)
	{
		if (forwardOutHit.GetComponent()->ComponentHasTag(FName(TEXT("Grappable"))))
		{
			EndTarget = forwardOutHit;

			//Grapple cable
			Cable->SetVisibility(true);
			Cable->SetAttachEndTo(EndTarget.GetActor(), "None", "None");
			Cable->EndLocation = VectorZero;
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Impact Point: %s"), *EndTarget.ImpactPoint.ToString()));

			// 
			MyActorComponent->GravityScale = 0;
			//MyActorComponent->Velocity = VectorZero;

			//Type 1: Grapple
			//MyActorComponent->AddImpulse((FVector(EndTarget.ImpactPoint.X, EndTarget.ImpactPoint.Y, EndTarget.ImpactPoint.Z + 1000.0f) - MyActorComponent->GetActorLocation()) * 100.0f, false);
			IsGrappling = true;

			//Type 2: Grapple
			// Linearly Interpolate above the box
			// if destination reached, return values to normal
			// MyActorComponent->SetActorLocation(FVector.Lerp(FVector(MyActorComponent->GetActorLocation(), EndTarget.ImpactPoint.X, EndTarget.ImpactPoint.Y, EndTarget.ImpactPoint.Z + 1000.0f)), TimeElapsed / LerpDuration);
		}
	}
}

void AMovementsCharacter::StopGrappling()
{
	Cable->SetVisibility(false);
	Cable->SetAttachEndTo(NULL, "test", "test");
	Cable->EndLocation = VectorZero;
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("CantGrapple")));
	IsGrappling = false;
}

FVector AMovementsCharacter::StartCameraLockOn()
{
	FVector GunStart = FP_Gun->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector ForwardEnd = (GunStart + (ForwardVector * 5000.0f));
	FHitResult forwardOutHit;
	FCollisionQueryParams CollisionParams;

	CanLockOn = GetWorld()->LineTraceSingleByChannel(forwardOutHit, GunStart, ForwardEnd, ECC_Visibility, CollisionParams);
	DrawDebugLine(GetWorld(), GunStart, ForwardEnd, FColor::Green, false, 1, 0, 1);

	if (CanLockOn)
	{
		IsCamLocked = true;
		return forwardOutHit.ImpactPoint;
	}
	return FVector::ZeroVector;
}

void AMovementsCharacter::OnWallRun()
{
	if (CanWallJump)
	{
		OnWallJump();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("CantWallJump")));
		if (WallDetector->CanWallRunBool) {
			StartWallRun();
		} 
	}
}

void AMovementsCharacter::OnWallJump()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("WallJump")));
	IsWallRunning = false;
	MyActorComponent->AddImpulse((WallDetector->NormalVector * NormalForce + (FirstPersonCameraComponent->GetForwardVector() * ForwardForce) + (FirstPersonCameraComponent->GetUpVector() * UpForce)) * ForceMultiplier, true);
}


void AMovementsCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMovementsCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMovementsCharacter::StartRun()
{
	MyActorComponent->MaxWalkSpeed = 600.0f; //needs to be a float
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Running")));
	isRunning = true;
}

void AMovementsCharacter::StopRun()
{
	MyActorComponent->MaxWalkSpeed = 400.0f; //needs to be a float
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Walking")));
	isRunning = false;
}

void AMovementsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMovementsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMovementsCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMovementsCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMovementsCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMovementsCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

// overlap on begin function
// void AMovementsCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	if (OtherActor && (OtherActor != this) && OtherComp && OtherActor->GetClass()->IsChildOf(ASwingDoor::StaticClass()))
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, TEXT("You are within collision"));
// 		CurrentDoor = Cast<ASwingDoor>(OtherActor);
// 	}
// }

// // overlap on end function
// void AMovementsCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	if (OtherActor && (OtherActor != this) && OtherComp)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, TEXT("You are exitting collision"));
// 		CurrentDoor = NULL;
// 	}
// }