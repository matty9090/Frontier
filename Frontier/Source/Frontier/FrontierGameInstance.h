// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FrontierPlayerState.h"
#include "FrontierGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UFrontierGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    FUniqueNetIdRepl WinningPlayer;
    TArray<FPlayerStats> Stats;
};
