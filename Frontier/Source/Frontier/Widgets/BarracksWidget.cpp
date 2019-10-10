// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "BarracksWidget.h"
#include "FrontierPlayerState.h"
#include "Buildings/Barracks.h"

void UBarracksWidget::NativeConstruct()
{
    ResearchTreeChanged(EResearchTreeChangedType::Unknown, nullptr);
    UnitQueueChanged();

    auto PS = GetOwningPlayerState<AFrontierPlayerState>();
    auto Barracks = Cast<ABarracks>(BuildingActor);

    ResearchTreeChangedHandle = PS->OnResearchTreeChangedEvent.AddUObject(this, &UBarracksWidget::ResearchTreeChanged);
    
    if (Barracks)
    {
        UnitQueueChangedHandle = Barracks->OnUnitQueueChanged().AddUObject(this, &UBarracksWidget::UnitQueueChanged);
    }
}