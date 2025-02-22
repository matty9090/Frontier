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
	void GameOver();

    // Commands
    UFUNCTION(Exec, Category="ExecFunctions")
    void DebugCheats();

    bool bCheats = false;

protected:
    UFUNCTION(BlueprintCallable)
    void SetupPlayer(AController* Player, FVector Location);

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AFrontierCharacter> WorkerClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ATownHall> TownHallClass;

    UPROPERTY()
    TArray<FVector> AvailableSpots;
};
