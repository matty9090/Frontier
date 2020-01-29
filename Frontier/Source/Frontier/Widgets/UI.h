// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Buildings/Building.h"
#include "Research.h"
#include "UI.generated.h"

class UUnitSelected;

/**
 * 
 */
UCLASS()
class FRONTIER_API UUI : public UUserWidget
{
    GENERATED_BODY()
    
public:
    bool Initialize() override;

    UFUNCTION(BlueprintImplementableEvent)
    void ShowUI();

    UFUNCTION(BlueprintImplementableEvent)
    void HideUI();

    UFUNCTION(BlueprintImplementableEvent)
    void ShowUnitUI();

    UFUNCTION(BlueprintImplementableEvent)
    void HideUnitUI();

    UPROPERTY(BlueprintReadWrite)
    bool bIsHidden = false;

    UPROPERTY(Meta=(BindWidget))
    UUnitSelected* UnitSelected;

protected:
    void ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node);
    void UpdateFilters();

    UFUNCTION(BlueprintCallable)
    void UpdateBuildingList();

    UFUNCTION(BlueprintImplementableEvent)
    void BindBuildingSelectedEvent(UUserWidget* Widget, TSubclassOf<ABuilding> Building);

    UPROPERTY(EditAnywhere)
    FMargin BuildingPadding = { 0.0f, 0.0f, 20.0f, 0.0f};

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> BuildingWidget;

    UPROPERTY(BlueprintReadWrite, Meta=(BindWidget))
    UScrollBox* BuildingsContainer;

    UPROPERTY(Meta=(BindWidget))
    UWidget* FilterGeneral;

    UPROPERTY(Meta=(BindWidget))
    UWidget* FilterResources;

    UPROPERTY(Meta=(BindWidget))
    UWidget* FilterCombat;

    UPROPERTY(BlueprintReadWrite)
    EBuildingCategory ActiveCategory = EBuildingCategory::General;

private:
    FDelegateHandle ResearchTreeChangedHandle;

    TArray<TSubclassOf<ABuilding>> Buildings;
};
