// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "UI.h"
#include "FrontierPlayerState.h"

bool UUI::Initialize()
{
    Super::Initialize();

    ResearchTreeChanged(EResearchTreeChangedType::Unknown, nullptr);

    auto PS = GetOwningPlayerState<AFrontierPlayerState>();

    if (PS)
    {
        ResearchTreeChangedHandle = PS->OnResearchTreeChangedEvent.AddUObject(this, &UUI::ResearchTreeChanged);
        return true;
    }

    return false;
}