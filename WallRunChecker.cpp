// Fill out your copyright notice in the Description page of Project Settings.

#include "WallRunChecker.h"
#include "MovementsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UWallRunChecker::UWallRunChecker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	byWall = false;
	frontWall = false;
	backWall = false;
	
	aboveThreshold = false;
	CanWallRunBool = false;
	NormalVector;
}


// Called when the game starts
void UWallRunChecker::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWallRunChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Origin
	FVector Start = GetOwner()->GetActorLocation();

	//Right and left
	FHitResult rightOutHit;
	FHitResult leftOutHit;
	FVector RightVector = Cast<UCameraComponent>(GetOwner()->GetDefaultSubobjectByName("FirstPersonCamera"))->GetRightVector();
	FVector RightEnd = (Start + (RightVector * 60.0f));
	FVector LeftEnd = (Start + (RightVector * -60.0f));

	//up and down
	FHitResult upOutHit;
	FHitResult downOutHit;
	FVector UpVector = GetOwner()->GetActorUpVector();
	FVector UpEnd = (Start + (UpVector * 150.0f));
	FVector BottomEnd = (Start + (UpVector * -100.0f));

	//forward and back
	FHitResult forwardOutHit;
	forwardNormal;
	FHitResult backOutHit;

	FVector FrontVector = GetOwner()->GetActorForwardVector();
	FVector FrontEnd = (Start + (FrontVector * 60.0f));
	FVector BackEnd = (Start + (FrontVector * -60.0f));

	FCollisionQueryParams CollisionParams;

	//Right and left
	bool rightWall = GetWorld()->LineTraceSingleByChannel(rightOutHit, Start, RightEnd, ECC_Visibility, CollisionParams);
	bool leftWall = GetWorld()->LineTraceSingleByChannel(leftOutHit, Start, LeftEnd, ECC_Visibility, CollisionParams);

	//forward and back
	bool forwardWall = GetWorld()->LineTraceSingleByChannel(forwardOutHit, Start, FrontEnd, ECC_Visibility, CollisionParams);
	bool backsideWall = GetWorld()->LineTraceSingleByChannel(backOutHit, Start, BackEnd, ECC_Visibility, CollisionParams);

	//up and down
	bool onGround = GetWorld()->LineTraceSingleByChannel(upOutHit, Start, BottomEnd, ECC_Visibility, CollisionParams);
	bool ceilingChecker = GetWorld()->LineTraceSingleByChannel(downOutHit, Start, UpEnd, ECC_Visibility, CollisionParams);
	
	// DrawDebugLine(GetWorld(), Start, RightEnd, FColor::Green, false, 1, 0, 1);
	// DrawDebugLine(GetWorld(), Start, LeftEnd, FColor::Green, false, 1, 0, 1);
	// DrawDebugLine(GetWorld(), Start, UpEnd, FColor::Green, false, 1, 0, 1);
	// DrawDebugLine(GetWorld(), Start, BottomEnd, FColor::Green, false, 1, 0, 1);

	if(rightWall)
	{
		if (rightOutHit.bBlockingHit)
		{
			if(GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("You are hitting: %s"), *rightOutHit.GetActor()->GetName()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Impact Point: %s"), *rightOutHit.ImpactPoint.ToString()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Normal Point: %s"), *rightOutHit.ImpactNormal.ToString()));
				byWall = true;
			}
			NormalVector = rightOutHit.Normal;
		}
	}

	if(leftWall)
	{
		if (leftOutHit.bBlockingHit)
		{
			if(GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("You are hitting: %s"), *leftOutHit.GetActor()->GetName()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Impact Point: %s"), *leftOutHit.ImpactPoint.ToString()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Normal Point: %s"), *leftOutHit.ImpactNormal.ToString()));
				byWall = true;
			}
			NormalVector = leftOutHit.Normal;
		}
	}

	if (forwardWall)
	{
		if (forwardOutHit.bBlockingHit)
		{
			if (GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("You are hitting: %s"), *leftOutHit.GetActor()->GetName()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Impact Point: %s"), *leftOutHit.ImpactPoint.ToString()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Normal Point: %s"), *leftOutHit.ImpactNormal.ToString()));
				frontWall = true;
				forwardNormal = forwardOutHit.Normal;
			}
			NormalVector = forwardOutHit.Normal;
		} 
	}

	if (backsideWall)
	{
		if (backOutHit.bBlockingHit)
		{
			if (GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("You are hitting: %s"), *leftOutHit.GetActor()->GetName()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Impact Point: %s"), *leftOutHit.ImpactPoint.ToString()));
				// GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, FString::Printf(TEXT("Normal Point: %s"), *leftOutHit.ImpactNormal.ToString()));
				backWall = true;
			}
			NormalVector = backOutHit.Normal;
		} 
	}

	if(!onGround)
	{
		aboveThreshold = true;
	} else {
		aboveThreshold = false;
	}

	if (!rightWall && !leftWall) {
		byWall = false;
	}

	if (!forwardWall) {
		frontWall = false;
	}

	if (!backsideWall) {
		backWall = false;
	}

	if (byWall && aboveThreshold)
	{
		CanWallRunBool = true;
	} else {
		CanWallRunBool = false;
	}
}