// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FrontierGameState.generated.h"

struct FPlayerStats;
class UResearchNode;
class AFrontierPlayerState;

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
