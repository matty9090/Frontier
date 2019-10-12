// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrontierGameMode.generated.h"

class ABuilding;
class AFrontierCharacter;

UCLASS(MinimalAPI)
class AFrontierGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AFrontierGameMode();

private:
    void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ABuilding> StartBuildingClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AFrontierCharacter> WorkerClass;
};
