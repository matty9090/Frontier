// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "UnitSelected.h"
#include "FrontierCharacter.h"

void UUnitSelected::SetSelectedUnits(const TArray<AFrontierCharacter*> SelectedUnits)
{
    Units = SelectedUnits;
    Num->SetText(FText::FromString(FString::FromInt(Units.Num())));

    TxtUnitType->SetText(FText::FromString(SelectedUnits[0]->UnitName));
    UnitList->ClearChildren();
        
    for (auto Unit : SelectedUnits)
    {
        UWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), UnitWidget);
        UnitList->AddChildToWrapBox(Widget);
    }
}