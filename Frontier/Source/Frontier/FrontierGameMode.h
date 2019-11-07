// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrontierGameMode.generated.h"

class ACity;
class ABuilding;
class AFrontierCharacter;

UCLASS(MinimalAPI)
class AFrontierGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AFrontierGameMode();

    void BeginPlay() override;

private:
    void InitPlayers();
    void ClearMap();
    void RestartPlayer(AController* NewPlayer) override;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AFrontierCharacter> WorkerClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ACity> CityClass;

    UPROPERTY()
    TArray<AController*> PlayerControllers;

    UPROPERTY()
    TArray<AActor*> SpawnedStartActors;
};
