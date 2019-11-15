// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrontierGameMode.generated.h"

class ABuilding;
class ATownHall;
class AFrontierCharacter;

UCLASS(MinimalAPI)
class AFrontierGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AFrontierGameMode();

    void BeginPlay() override;

    // Commands
    UFUNCTION(Exec, Category="ExecFunctions")
    void DebugCheats();

    bool bCheats = false;

private:
    void InitPlayers();
    void ClearMap();
    void RestartPlayer(AController* NewPlayer) override;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AFrontierCharacter> WorkerClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ATownHall> TownHallClass;

    UPROPERTY()
    TArray<AController*> PlayerControllers;

    UPROPERTY()
    TArray<AActor*> SpawnedStartActors;
};
