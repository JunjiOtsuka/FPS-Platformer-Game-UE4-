// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetPlayerPosition.h"

// Sets default values
AResetPlayerPosition::AResetPlayerPosition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetResetPosition;
}

// Called when the game starts or when spawned
void AResetPlayerPosition::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResetPlayerPosition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

