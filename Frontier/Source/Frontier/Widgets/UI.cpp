// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "UI.h"
#include "Frontier.h"
#include "FrontierPlayerState.h"

bool UUI::Initialize()
{
    Super::Initialize();

    auto PS = GetOwningPlayerState<AFrontierPlayerState>();

    if (PS)
    {
        ResearchTreeChanged(EResearchTreeChangedType::Unknown, nullptr);
        ResearchTreeChangedHandle = PS->OnResearchTreeChangedEvent.AddUObject(this, &UUI::ResearchTreeChanged);

        return true;
    }

    return false;
}

void UUI::ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node)
{
    Buildings = GetOwningPlayerState<AFrontierPlayerState>()->GetResearchedBuildings();
    
    UpdateFilters();
    UpdateBuildingList();
}

void UUI::UpdateFilters()
{
    bool hasGeneral = false, hasRes = false, hasCombat = false;

    for (auto Building : Buildings)
    {
        auto DefaultObj = Building.GetDefaultObject();

        hasGeneral |= DefaultObj->BuildingCategory == EBuildingCategory::General;
        hasRes |= DefaultObj->BuildingCategory == EBuildingCategory::Resource;
        hasCombat |= DefaultObj->BuildingCategory == EBuildingCategory::Combat;
    }

    FilterGeneral->SetVisibility(hasGeneral ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    FilterResources->SetVisibility(hasRes ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    FilterCombat->SetVisibility(hasCombat ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UUI::UpdateBuildingList()
{
    BuildingsContainer->ClearChildren();

    for (auto Building : Buildings)
    {
        auto DefaultObj = Building.GetDefaultObject();
        
        if (DefaultObj->BuildingCategory != ActiveCategory)
            continue;

        auto Widget = CreateWidget(GetOwningPlayer(), BuildingWidget);
        Widget->SetPadding(BuildingPadding);

        BindBuildingSelectedEvent(Widget, Building);
        BuildingsContainer->AddChild(Widget);
    }
}
