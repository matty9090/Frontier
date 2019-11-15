// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierGameMode.h"
#include "FrontierPlayerController.h"
#include "FrontierPlayerState.h"
#include "FrontierCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"
#include "Buildings/Building.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Buildings/TownHall.h"
#include "City.h"
#include "Frontier.h"

AFrontierGameMode::AFrontierGameMode()
{

}

void AFrontierGameMode::BeginPlay()
{

}

void AFrontierGameMode::DebugCheats()
{
    bCheats = true;

    FResources Res(99999, 99999, 99999, 99999, 99999, -1, 99999);

    for (TObjectIterator<APlayerState> It; It; ++It)
    {
        Cast<AFrontierPlayerState>(*It)->AddResources(Res);
    }
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

        auto BoxComponent = Cast<UBoxComponent>(TownHallClass.GetDefaultObject()->GetComponentByClass(UBoxComponent::StaticClass()));
        float Z = BoxComponent->GetScaledBoxExtent().Z;
        FTransform TownHallTransform(Location + FVector(0.0f, 0.0f, Z));

        auto TownHall = GetWorld()->SpawnActorDeferred<ATownHall>(TownHallClass, TownHallTransform, PS, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
        TownHall->Player = PS;
        TownHall->bBuilt = true;

        FTransform WorkerTransform(Location + FVector(300.0f, 0.0f, 100.0f));
        auto Worker = GetWorld()->SpawnActorDeferred<AFrontierCharacter>(WorkerClass, WorkerTransform, PS, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
        Worker->Player = PS;

        UGameplayStatics::FinishSpawningActor(TownHall, TownHallTransform);
        UGameplayStatics::FinishSpawningActor(Worker, WorkerTransform);

        SpawnedStartActors.Add(TownHall);
        SpawnedStartActors.Add(Worker);
    }

    UE_LOG(LogFrontier, Display, TEXT("Initialised world"));
}

void AFrontierGameMode::ClearMap()
{
    for (auto Actor : SpawnedStartActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
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
