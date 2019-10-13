// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "UnitQueueCommonWidget.h"
#include "FrontierPlayerState.h"
#include "Buildings/UnitQueueCommon.h"

void UUnitQueueCommonWidget::NativeConstruct()
{
    ResearchTreeChanged(EResearchTreeChangedType::Unknown, nullptr);
    UnitQueueChanged();

    auto PS = GetOwningPlayerState<AFrontierPlayerState>();
    auto Barracks = Cast<AUnitQueueCommon>(BuildingActor);

    ResearchTreeChangedHandle = PS->OnResearchTreeChangedEvent.AddUObject(this, &UUnitQueueCommonWidget::ResearchTreeChanged);
    
    if (Barracks)
    {
        UnitQueueChangedHandle = Barracks->OnUnitQueueChanged().AddUObject(this, &UUnitQueueCommonWidget::UnitQueueChanged);
    }
}