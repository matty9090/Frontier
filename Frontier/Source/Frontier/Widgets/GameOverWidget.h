// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UStatsWidget;

/**
 * 
 */
UCLASS()
class FRONTIER_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

    void NativeConstruct() override;
	
private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UStatsWidget> StatsWidgetClass;

    UPROPERTY(Meta = (BindWidget))
    UTextBlock* TxtTitle;

    UPROPERTY(Meta = (BindWidget))
    UHorizontalBox* Container;
};
