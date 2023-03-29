// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlapHandler.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TargetResetLocation.h"
#include "UObject/UObjectGlobals.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UOverlapHandler::UOverlapHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	BoxComp->InitBoxExtent(FVector(100, 100, 100));
	BoxComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	BoxComp->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	BoxComp->SetGenerateOverlapEvents(true);
	//RootComponent = BoxComp;
}


// Called when the game starts
void UOverlapHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	DrawDebugBox(GetWorld(), GetOwner()->GetActorLocation(), BoxComp->GetScaledBoxExtent(), FQuat(GetOwner()->GetActorRotation()), FColor::Turquoise, true, 999, 0, 2);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &UOverlapHandler::OnOverlapBegin);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &UOverlapHandler::OnOverlapEnd);
}


// Called every frame
void UOverlapHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOverlapHandler::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp || OtherActor || OtherComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Inside")));
	}
}

// overlap on end function
void UOverlapHandler::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Outside")));
	}
}