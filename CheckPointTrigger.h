// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerBox.h"
#include "CheckPointTrigger.generated.h"

/**
 * 
 */
UCLASS()
class MOVEMENTS_API ACheckPointTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// constructor sets default values for this actor's properties
	ACheckPointTrigger();

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* OnMaterial;

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* OffMaterial;

	UPROPERTY(EditAnywhere)
		bool IsTriggered;

	// overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	// overlap end function
	UFUNCTION()
		void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Mesh")
	class AActor* CheckPointMeshRenderer;
};