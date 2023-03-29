// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MovementsGameMode.h"
#include "MovementsHUD.h"
#include "MovementsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMovementsGameMode::AMovementsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMovementsHUD::StaticClass();
}
