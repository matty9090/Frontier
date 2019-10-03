// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBaseWidget.h"
#include "Research.h"
#include "BarracksWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UBarracksWidget : public UBuildingBaseWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct() override;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node);

private:
    FDelegateHandle ResearchTreeChangedHandle;
};
