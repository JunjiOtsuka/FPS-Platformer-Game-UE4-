// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunChecker.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MOVEMENTS_API UWallRunChecker : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWallRunChecker();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector NormalVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool byWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool frontWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool backWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector forwardNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool aboveThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool CanWallRunBool;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
