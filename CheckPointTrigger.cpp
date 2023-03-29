// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPointTrigger.h"

#include "DrawDebugHelpers.h"

ACheckPointTrigger::ACheckPointTrigger()
{
	//Register Events
	OnActorBeginOverlap.AddDynamic(this, &ACheckPointTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ACheckPointTrigger::OnOverlapEnd);

	OnMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("M_Ground_Grass"));
	//OffMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("M_Ground_Grass"));

	CheckPointMeshRenderer;

	IsTriggered = false;
}

// Called when the game starts or when spawned
void ACheckPointTrigger::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, 999, 0, 5);

	//CheckPointMeshRenderer = FindObject<AActor>(GetOuter(), TEXT("TargetLocation"));
}

void ACheckPointTrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	// check if Actors do not equal nullptr and that 
	if (OtherActor && OtherActor != this && !this->IsTriggered)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, TEXT("Overlap Begin"));
			//CheckPointMeshRenderer->GetActorComponents<UStaticMeshComponent>();
			CheckPointMeshRenderer->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, OnMaterial);
			CheckPointMeshRenderer->SetActorLocationAndRotation(this->GetActorLocation(), this->GetActorRotation());
			this->IsTriggered = true;
		}
	}
}

void ACheckPointTrigger::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, TEXT("Overlap Ended"));
			this->IsTriggered = false;
		}
	}
}
