// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "UnitQueueItemWidget.h"

void UUnitQueueItemWidget::BroadcastClick()
{
    UnitClickedEvent.Broadcast(Index, Unit);
}