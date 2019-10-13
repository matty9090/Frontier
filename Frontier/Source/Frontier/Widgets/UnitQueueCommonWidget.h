// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBaseWidget.h"
#include "Research.h"
#include "Buildings/UnitQueueCommon.h"
#include "UnitQueueCommonWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UUnitQueueCommonWidget : public UBuildingBaseWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct() override;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node);

    UFUNCTION(BlueprintImplementableEvent)
    void UnitQueueChanged();

private:
    FDelegateHandle ResearchTreeChangedHandle;
    FDelegateHandle UnitQueueChangedHandle;
};
