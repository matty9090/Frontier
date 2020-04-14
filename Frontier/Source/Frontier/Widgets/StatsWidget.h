// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "FrontierPlayerState.h"
#include "StatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdateStats(FPlayerStats Stats);
	
private:
    UPROPERTY(Meta = (BindWidget))
    UTextBlock* YouLabel;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtTotalResources;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtBuildingsBuilt;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtLargestArmy;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtUnitsKilled;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtUnitsCreated;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtAPM;
};
