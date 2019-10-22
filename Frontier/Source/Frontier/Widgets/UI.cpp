// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "UI.h"
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
    UpdateBuildingList();
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
