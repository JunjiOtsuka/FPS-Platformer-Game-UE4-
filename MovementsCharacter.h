// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "GameFramework/Character.h"
#include "Tactical1.h"
#include "Tactical2.h"

#include "MovementsCharacter.generated.h"

class UInputComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTacticalDelegate);
//DECLARE_DELEGATE(FTacticalDelegateS);

UCLASS(config = Game)

class AMovementsCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

	UPROPERTY(VisibleAnywhere, Category = "Trigger Capsule")
		class UCapsuleComponent* TriggerCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
		class UCableComponent* Cable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WallDetector, meta = (AllowPrivateAccess = "true"))
		class UWallRunChecker* WallDetector;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GrapplingHook, meta = (AllowPrivateAccess = "true"))
	// class UGrapplingHook* GrapplingHook;

	

public:
	AMovementsCharacter();

protected:
	virtual void BeginPlay();

public:
	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector SavedVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector SavedHitPoint;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AMovementsProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint32 bUsingMotionControllers : 1;

	// // declare overlap begin function
	// UFUNCTION()
	// void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// // declare overlap end function
	// UFUNCTION()
	// void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float NormalForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float ForwardForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float UpForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float ForceMultiplier;

	UFUNCTION()
		FVector StartCameraLockOn();

	UFUNCTION()
		void bCheckDelegateSubscription();

	UFUNCTION()
		void StartWallRun();

	UFUNCTION()
		void StopWallRun();

	UFUNCTION()
		void StartWallAction();

	UFUNCTION()
		void StopWallAction();

	UFUNCTION()
		void OnWallJump();

	UFUNCTION()
		void StartGrappling();

	UFUNCTION()
		void StopGrappling();

	UFUNCTION()
		void Quit();

	UFUNCTION()
		void OnTimerOver();

	// UPROPERTY()
	// class ASwingDoor* CurrentDoor;

	UPROPERTY()
		class UCharacterMovementComponent* MyActorComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsWallRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool CanWallJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool CanLockOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool CanGrapple;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsGrappling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsCamLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bWallStick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bTimerStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool UpdatedEndTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bForwardInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bBackInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bRightInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bLeftInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FHitResult EndTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector VectorZero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float Force;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float TimeElapsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float LerpDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float MaxFuzeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bTimerOver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FTimerHandle FuzeTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WallDetector, meta = (AllowPrivateAccess = "true"))
		ATactical1* m_Tactical_Type_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WallDetector, meta = (AllowPrivateAccess = "true"))
		ATactical2* m_Tactical_Type_2;

	UPROPERTY(BlueprintAssignable)
		FTacticalDelegate m_TacticalDelegate;

	UPROPERTY()
		TArray<TScriptInterface<ITactical>> m_dInterface;

	//UPROPERTY(BlueprintAssignable)
		//FTacticalDelegateS m_TacticalDelegateS;

	/*
	#include "Tactical.h"

	UINTERFACE(MinimalAPI)
		ITactical* ITactical;

		void DoTactical() override;
	*/

protected:

	/** Fires a projectile. */
	void OnFire();

	//Wallrun logic here
	void OnWallRun();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/** Handles faster moving forward/backward */
	void StartRun();

	/** Handles faster stafing movement, left and right */
	void StopRun();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

