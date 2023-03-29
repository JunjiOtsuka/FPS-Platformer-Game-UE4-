// Fill out your copyright notice in the Description page of Project Settings.


#include "Tactical1.h"
#include "Engine.h"
#include "GameFramework/Actor.h"

// Sets default values
ATactical1::ATactical1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATactical1::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATactical1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATactical1::DoTactical_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Do Tactical Type 1")));
}

void ATactical1::BindToDelegate()
{
	DoTactical_Implementation();
}
