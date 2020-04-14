// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "GameOverWidget.h"
#include "StatsWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "FrontierGameInstance.h"

void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    auto GI = GetGameInstance<UFrontierGameInstance>();
    auto Stats = GI->Stats;
    bool DidWin = GI->GetPrimaryPlayerUniqueId() == GI->WinningPlayer;

    Container->ClearChildren();
    TxtTitle->SetText(FText::FromString(DidWin ? "Victory" : "Defeat"));
    TxtTitle->SetColorAndOpacity(FSlateColor(DidWin ? FLinearColor::Green : FLinearColor::Red));

    for (auto Stat : Stats)
    {
        auto StatsWidget = CreateWidget<UStatsWidget>(GetOwningPlayer(), StatsWidgetClass);
        StatsWidget->UpdateStats(Stat);
        StatsWidget->Padding.Right = 20;
        StatsWidget->Padding.Left = 20;
        
        auto BoxSlot = Container->AddChildToHorizontalBox(StatsWidget);
        BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        BoxSlot->SynchronizeProperties();
    }
}
