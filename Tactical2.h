// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/Interface.h"
#include "Tactical.h"

#include "Tactical2.generated.h"

UCLASS()
class MOVEMENTS_API ATactical2 : public AActor, public ITactical
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATactical2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DoTactical_Implementation() override;

};
