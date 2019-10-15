// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierGameMode.h"
#include "FrontierPlayerController.h"
#include "FrontierPlayerState.h"
#include "FrontierCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"
#include "Buildings/Building.h"
#include "EngineUtils.h"
#include "Frontier.h"

AFrontierGameMode::AFrontierGameMode()
{

}

void AFrontierGameMode::BeginPlay()
{

}

void AFrontierGameMode::InitPlayers()
{
    TArray<FVector> AvailableSpots;

    for (TObjectIterator<APlayerStart> It; It; ++It)
    {
        if (!AvailableSpots.Contains((*It)->GetActorLocation()))
        {
            AvailableSpots.Add((*It)->GetActorLocation());
        }
    }

    for (auto Player : PlayerControllers)
    {
        if (AvailableSpots.Num() <= 0)
        {
            Logout(Player);
            UE_LOG(LogFrontier, Warning, TEXT("Not enough start spots! Kicking player."));
            break;
        }

        auto Location = AvailableSpots.Pop();
        Location.Z = 0;

        Player->GetPawn()->SetActorLocation(Location);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        auto PS = Cast<AFrontierPlayerState>(Player->PlayerState);
        auto BoxComponent = Cast<UBoxComponent>(StartBuildingClass.GetDefaultObject()->GetComponentByClass(UBoxComponent::StaticClass()));
        
        float Z = BoxComponent->GetScaledBoxExtent().Z;

        auto StartBuilding = GetWorld()->SpawnActor<ABuilding>(StartBuildingClass, Location + FVector(0.0f, 0.0f, Z), FRotator::ZeroRotator, SpawnParams);
        StartBuilding->SetOwner(PS);
        StartBuilding->Player = PS;

        auto Worker = GetWorld()->SpawnActor<AFrontierCharacter>(WorkerClass, Location + FVector(300.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
        Worker->SetOwner(PS);
        Worker->Player = PS;

        SpawnedStartActors.Add(StartBuilding);
        SpawnedStartActors.Add(Worker);
    }

    UE_LOG(LogFrontier, Warning, TEXT("Initialised world"));
}

void AFrontierGameMode::ClearMap()
{
    for (auto Actor : SpawnedStartActors)
    {
        Actor->Destroy();
    }

    SpawnedStartActors.Empty();
}

void AFrontierGameMode::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);

    PlayerControllers.Add(NewPlayer);

    ClearMap();
    InitPlayers();
}
