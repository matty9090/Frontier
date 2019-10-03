// Fill out your copyright notice in the Description page of Project Settings.


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