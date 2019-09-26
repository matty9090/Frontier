// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
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

void AFrontierPlayerController::OnRep_PlacedBuilding()
{
    if (SelectedBuilding)
    {
        SelectedBuilding->Destroy();
    }
}

bool AFrontierPlayerController::ServerSpawnBuilding_Validate(UClass* Type, FVector Location, FRotator Rotation)
{
    return true;
}

void AFrontierPlayerController::ServerSpawnBuilding_Implementation(UClass* Type, FVector Location, FRotator Rotation)
{
    if (Type)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        PlacedBuilding = GetWorld()->SpawnActor<ABuilding>(
            Type,
            Location,
            Rotation,
            SpawnParams
        );

        if (GetNetMode() == ENetMode::NM_ListenServer && SelectedBuilding)
        {
            SelectedBuilding->Destroy();
        }
    }
}

bool AFrontierPlayerController::ServerMoveAIToLocation_Validate(AFrontierCharacter* AI, FVector Location, AActor* Resource)
{
    return true;
}

void AFrontierPlayerController::ServerMoveAIToLocation_Implementation(AFrontierCharacter* AI, FVector Location, AActor* Object)
{
    AI->MoveToLocation(Location, Object);
}

void AFrontierPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerController, PlacedBuilding);
}