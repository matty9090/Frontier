// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Engine/World.h"
#include "Building.h"
#include "Research.h"
#include "Frontier.h"

AFrontierPlayerController::AFrontierPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
}

void AFrontierPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
    {
        ClientCreateUI();
    }
}

void AFrontierPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
}

void AFrontierPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AFrontierPlayerController::OnRep_PlacedBuilding()
{
    if (HoveredBuilding)
    {
        HoveredBuilding->Destroy();
    }
}

void AFrontierPlayerController::ClientCreateUI_Implementation()
{
    CreateUI();
}

void AFrontierPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    CreateUI();
}

bool AFrontierPlayerController::ServerSpawnBuilding_Validate(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation)
{
    return Cast<AFrontierPlayerState>(PlayerState)->CanCreateBuilding(Type);
}

void AFrontierPlayerController::ServerSpawnBuilding_Implementation(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation)
{
    // TODO: Move to PlayerState?
    if (Type)
    {
        auto PS = Cast<AFrontierPlayerState>(PlayerState);
        PS->Resources -= Type.GetDefaultObject()->Cost;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        PlacedBuilding = GetWorld()->SpawnActor<ABuilding>(
            Type,
            Location,
            Rotation,
            SpawnParams
        );

        PlacedBuilding->Player = PS;

        if (GetNetMode() == ENetMode::NM_ListenServer && HoveredBuilding)
        {
            HoveredBuilding->Destroy();
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

bool AFrontierPlayerController::ServerQueueUnit_Validate(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    return Building->CanCreateUnit(Unit);
}

void AFrontierPlayerController::ServerQueueUnit_Implementation(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    Building->QueueUnit(Unit);
}

bool AFrontierPlayerController::ServerResearch_Validate(UResearchNode* Node)
{
    return Cast<AFrontierPlayerState>(PlayerState)->CanResearchNode(Node);
}

void AFrontierPlayerController::ServerResearch_Implementation(UResearchNode* Node)
{
    Cast<AFrontierPlayerState>(PlayerState)->Research(Node);
}

void AFrontierPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerController, PlacedBuilding);
}