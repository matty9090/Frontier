// Fill out your copyright notice in the Description page of Project Settings.


#include "ResearchNodeWidget.h"
#include "TextBlock.h"
#include "Research.h"

void UResearchNodeWidget::NativeConstruct()
{
    TxtName->SetText(FText::FromString(Node->Name));
}
