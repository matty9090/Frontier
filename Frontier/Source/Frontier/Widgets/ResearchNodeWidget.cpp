// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "ResearchNodeWidget.h"
#include "TextBlock.h"
#include "Research.h"

void UResearchNodeWidget::NativeConstruct()
{
    
}

void UResearchNodeWidget::UpdateNodeProperties()
{
    if (Node)
    {
        TxtName->SetText(FText::FromString(Node->Name));
    }
}