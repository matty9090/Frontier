// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingBaseWidget.generated.h"

class ABuilding;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class FRONTIER_API UBuildingBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    UWidgetAnimation* GetShowAnimation() const;

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    UWidgetAnimation* GetHideAnimation() const;

    UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn))
    ABuilding* BuildingActor;
};
