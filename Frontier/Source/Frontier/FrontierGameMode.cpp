// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierGameMode.h"
#include "FrontierGameState.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "FrontierCharacter.h"
#include "FrontierGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"
#include "Buildings/Building.h"
#include "Components/HealthComponent.h"
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

void AFrontierGameMode::SetupPlayer(AController* Player, FVector Location)
{
    Location.Z = 0;
    Player->GetPawn()->SetActorLocation(Location);

    auto PS = Cast<AFrontierPlayerState>(Player->PlayerState);

    auto BoxComponent = TownHallClass.GetDefaultObject()->FindComponentByClass<UBoxComponent>();
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

    auto Health = TownHall->FindComponentByClass<UHealthComponent>();
    Health->SetHealth(Health->MaxHealth);
}

void AFrontierGameMode::GameOver()
{
    auto GS = GetGameState<AFrontierGameState>();
    float GameTime = GS->GetServerWorldTimeSeconds();

    FUniqueNetIdRepl Winner;
    TArray<FPlayerStats> Stats;

    for (auto Player : GS->FrontierPlayers)
    {
        if (Player->Cities.Num() > 0)
        {
            Winner = Player->UniqueId;
        }

        Player->PlayerStats.APM = static_cast<float>(Player->PlayerStats.Actions) / (GameTime / 60.0f);
        Player->PlayerStats.PlayerID = Player->UniqueId;
        Stats.Add(Player->PlayerStats);
    }

    TArray<AActor*> Controllers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFrontierPlayerController::StaticClass(), Controllers);

    for (auto Controller : Controllers)
    {
        Cast<AFrontierPlayerController>(Controller)->OnGameOver(Stats, Winner);
    }
}
