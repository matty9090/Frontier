// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Blueprint/UserWidget.h"
#include "UnitQueueItemWidget.generated.h"

class AFrontierCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUnitQueueItemClicked, int, Index, TSubclassOf<AFrontierCharacter>, Unit);

/**
 * 
 */
UCLASS()
class FRONTIER_API UUnitQueueItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void BroadcastClick();

    UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn))
    TSubclassOf<AFrontierCharacter> Unit;

    UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn))
    int Index;

	UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn))
    UTexture2D* Icon;

    UPROPERTY(BlueprintAssignable)
    FUnitQueueItemClicked UnitClickedEvent;
};
