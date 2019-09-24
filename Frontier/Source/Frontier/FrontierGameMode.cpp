// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierGameMode.h"
#include "FrontierPlayerController.h"
#include "FrontierCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFrontierGameMode::AFrontierGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFrontierPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Frontier/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}