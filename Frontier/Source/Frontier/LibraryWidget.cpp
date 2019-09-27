// Fill out your copyright notice in the Description page of Project Settings.


#include "LibraryWidget.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "Research.h"
#include "ResearchNodeWidget.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

void ULibraryWidget::NativeConstruct()
{
    auto PS = GetOwningPlayerState<AFrontierPlayerState>();
    ResearchRoot = PS->ResearchRootNode;

    if (ResearchRoot)
    {
        BuildTree(ResearchRoot, FVector2D(20, 400));
    }
    else
    {
        UE_LOG(LogFrontier, Error, TEXT("Research manager is not set!"));
    }
}

void ULibraryWidget::BuildTree(UResearchNode* Node, FVector2D Pos)
{
    if (Node)
    {
        CreateNodeAtPosition(ResearchRoot, Pos);
        
        Pos += FVector2D(XSpacing, -YSpacing);

        for (auto Child : Node->ChildNodes)
        {
            BuildTree(Child, Pos);
            Pos.Y += Size + YSpacing;
        }
    }
}

void ULibraryWidget::CreateNodeAtPosition(UResearchNode* Node, FVector2D Pos)
{
    auto Widget = CreateWidget<UResearchNodeWidget>(GetOwningPlayer(), ResearchNodeWidgetClass);
    Widget->Node = Node;
    Widget->UpdateNodeProperties();

    auto CanvasSlot = Canvas->AddChildToCanvas(Widget);
    CanvasSlot->bAutoSize = true;
    CanvasSlot->SetPosition(Pos);
    CanvasSlot->SynchronizeProperties();
}