// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBaseWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "UnitSelected.generated.h"

class AFrontierCharacter;

/**
 * 
 */
UCLASS()
class FRONTIER_API UUnitSelected : public UBuildingBaseWidget
{
	GENERATED_BODY()
	
public:
    void SetSelectedUnits(const TArray<AFrontierCharacter*> SelectedUnits);

protected:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> UnitWidget;

    UPROPERTY(Meta=(BindWidget))
    UTextBlock* Num;

    UPROPERTY(Meta=(BindWidget))
    UWrapBox* UnitList;

    UPROPERTY(Meta=(BindWidget))
    UTextBlock* TxtUnitType;

    UPROPERTY(BlueprintReadOnly)
    TArray<AFrontierCharacter*> Units;
};
