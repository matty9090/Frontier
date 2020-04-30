// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "StatsWidget.h"
#include "FrontierGameInstance.h"

void UStatsWidget::UpdateStats(FPlayerStats Stats)
{
    Super::NativeConstruct();

    int32 Res = 0;

    Res += Stats.TotalResources.Resources[EResource::Wood];
    Res += Stats.TotalResources.Resources[EResource::Stone];

    TxtTotalResources->Text = FText::FromString(FString::FromInt(Res));
    TxtBuildingsBuilt->Text = FText::FromString(FString::FromInt(Stats.BuildingsBuilt));
    TxtLargestArmy->Text = FText::FromString(FString::FromInt(Stats.LargestArmy));
    TxtUnitsKilled->Text = FText::FromString(FString::FromInt(Stats.UnitsKilled));
    TxtUnitsCreated->Text = FText::FromString(FString::FromInt(Stats.UnitsCreated));
    TxtAPM->Text = FText::FromString(FString::FromInt(static_cast<int32>(Stats.APM)));

    auto GI = GetGameInstance<UFrontierGameInstance>();
    
    if (GI->GetPrimaryPlayerUniqueId() == Stats.PlayerID)
    {
        YouLabel->SetVisibility(ESlateVisibility::Visible);
    }
}
