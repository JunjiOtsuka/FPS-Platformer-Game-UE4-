// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlapChecker.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TargetResetLocation.h"
#include "UObject/UObjectGlobals.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AOverlapChecker::AOverlapChecker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	BoxComp->InitBoxExtent(FVector(150, 100, 100));
	BoxComp->SetCollisionProfileName("Trigger");
	RootComponent = BoxComp;

}

// Called when the game starts or when spawned
void AOverlapChecker::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), BoxComp->GetScaledBoxExtent(), FQuat(GetActorRotation()), FColor::Turquoise, true, 999, 0, 2);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AOverlapChecker::OnOverlapBegin);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AOverlapChecker::OnOverlapEnd);

	ResetPosition = FindObject<AActor>(GetOuter(), TEXT("TargetLocation"));
}

// Called every frame
void AOverlapChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// overlap on begin function
void AOverlapChecker::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Inside")));
		OtherActor->TeleportTo(ResetPosition->GetActorLocation(), ResetPosition->GetActorRotation(), false, false);
		OtherActor->FindComponentByClass<UCharacterMovementComponent>()->Velocity = FVector(0.f, 0.f, 0.f);
	}
}

// overlap on end function
void AOverlapChecker::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, (TEXT("Outside")));
	}
}