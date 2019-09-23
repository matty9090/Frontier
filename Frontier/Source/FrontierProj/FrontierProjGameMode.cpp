// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierProjGameMode.h"
#include "FrontierProjPlayerController.h"
#include "FrontierProjCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFrontierProjGameMode::AFrontierProjGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFrontierProjPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}