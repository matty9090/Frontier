// Fill out your copyright notice in the Description page of Project Settings.


#include "BarracksWidget.h"
#include "FrontierPlayerState.h"

void UBarracksWidget::NativeConstruct()
{
    ResearchTreeChanged();

    auto PS = GetOwningPlayerState<AFrontierPlayerState>();
    ResearchTreeChangedHandle = PS->OnResearchTreeChangedEvent.AddUObject(this, &UBarracksWidget::ResearchTreeChanged);
}