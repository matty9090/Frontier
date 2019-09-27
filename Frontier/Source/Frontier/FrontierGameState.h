// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FrontierGameState.generated.h"

class AFrontierPlayerState;
class UResearchNode;

/**
 * 
 */
UCLASS()
class FRONTIER_API AFrontierGameState : public AGameStateBase
{
    GENERATED_BODY()
    
public:
    AFrontierGameState();

    void AddPlayerState(APlayerState* PlayerState) override;
    void RemovePlayerState(APlayerState* PlayerState) override;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TArray<AFrontierPlayerState*> FrontierPlayers;

    UPROPERTY(BlueprintReadOnly, Replicated)
    int32 NumTeams = 0;
};
