// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBaseWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "UnitSelected.generated.h"

class AFrontierCharacter;
class UUnitQueueItemWidget;

/**
 * 
 */
UCLASS()
class FRONTIER_API UUnitSelected : public UBuildingBaseWidget
{
	GENERATED_BODY()
	
public:
    UUnitSelected();

    void SetSelectedUnits(const TArray<AFrontierCharacter*> SelectedUnits);

protected:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUnitQueueItemWidget> UnitWidget;

    UPROPERTY(Meta=(BindWidget))
    UTextBlock* Num;

    UPROPERTY(Meta=(BindWidget))
    UWrapBox* UnitList;

    UPROPERTY(Meta=(BindWidget))
    UTextBlock* TxtUnitType;

    UPROPERTY(BlueprintReadOnly)
    TArray<AFrontierCharacter*> Units;

private:
    UFUNCTION()
    void OnUnitClicked(int Index, TSubclassOf<AFrontierCharacter> Unit);

    FScriptDelegate UnitClickedHandler;
};
