// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Buildings/Building.h"
#include "Research.h"
#include "UI.generated.h"

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

    UPROPERTY(BlueprintReadWrite)
    bool bIsHidden = false;

protected:
    void ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    void UpdateBuildingList();

    UFUNCTION(BlueprintImplementableEvent)
    void BindBuildingSelectedEvent(UUserWidget* Widget, TSubclassOf<ABuilding> Building);

    UPROPERTY(EditAnywhere)
    FMargin BuildingPadding = { 0.0f, 0.0f, 20.0f, 0.0f};

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> BuildingWidget;

    UPROPERTY(Meta=(BindWidget))
    UScrollBox* BuildingsContainer;

    UPROPERTY(BlueprintReadWrite)
    EBuildingCategory ActiveCategory = EBuildingCategory::General;

private:
    FDelegateHandle ResearchTreeChangedHandle;

    TArray<TSubclassOf<ABuilding>> Buildings;
};
