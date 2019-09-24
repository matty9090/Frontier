// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "FrontierCharacter.h"
#include "Engine/World.h"
#include "Building.h"
#include "Frontier.h"

AFrontierPlayerController::AFrontierPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AFrontierPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AFrontierPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

bool AFrontierPlayerController::SpawnBuildingOnServer_Validate(UClass* Type, FVector Location, FRotator Rotation)
{
    return true;
}

void AFrontierPlayerController::SpawnBuildingOnServer_Implementation(UClass* Type, FVector Location, FRotator Rotation)
{
    if (Type)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        GetWorld()->SpawnActor<ABuilding>(
            Type,
            Location,
            Rotation,
            SpawnParams
        );
    }
}