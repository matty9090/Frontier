// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "UnitSelected.h"
#include "FrontierCharacter.h"
#include "UnitQueueItemWidget.h"

UUnitSelected::UUnitSelected()
{
    UnitClickedHandler.BindUFunction(this, "OnUnitClicked");
}

void UUnitSelected::SetSelectedUnits(const TArray<AFrontierCharacter*> SelectedUnits)
{
    Units = SelectedUnits;
    Num->SetText(FText::FromString(FString::FromInt(Units.Num())));

    TxtUnitType->SetText(FText::FromString(SelectedUnits[0]->UnitName));
    UnitList->ClearChildren();
        
    for (auto Unit : SelectedUnits)
    {
        auto Widget = CreateWidget<UUnitQueueItemWidget>(GetOwningPlayer(), UnitWidget);
        Widget->Unit = Unit->GetClass();
		Widget->Icon = Unit->Icon.Texture;
        Widget->UnitClickedEvent.Add(UnitClickedHandler);
        UnitList->AddChildToWrapBox(Widget);
    }
}

void UUnitSelected::OnUnitClicked(int Index, TSubclassOf<AFrontierCharacter> Unit)
{
    TxtUnitType->SetText(FText::FromString(Unit.GetDefaultObject()->UnitName));
}
