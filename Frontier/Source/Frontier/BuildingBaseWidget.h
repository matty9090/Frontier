// Fill out your copyright notice in the Description page of Project Settings.

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
    UWidgetAnimation* GetToggleAnimation();

    UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn))
    ABuilding* BuildingActor;
};
