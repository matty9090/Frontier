// Fill out your copyright notice in the Description page of Project Settings.


#include "LibraryWidget.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "Rendering/DrawElements.h"
#include "ResearchNodeWidget.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

void ULibraryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    auto PS = GetOwningPlayerState<AFrontierPlayerState>();
    ResearchRoot = PS->ResearchRootNode;

    if (ResearchRoot)
    {
        BuildTree(ResearchRoot, NodeStartPosition, NodeStartPosition);
        InitTree();
    }
    else
    {
        UE_LOG(LogFrontier, Error, TEXT("Research manager is not set!"));
    }

    PS->OnResearchTreeChangedEvent.AddUObject(this, &ULibraryWidget::ResearchTreeChanged);
}

void ULibraryWidget::BuildTree(UResearchNode* Node, FVector2D ParentPos, FVector2D Pos)
{
    if (Node)
    {
        FVector2D InitialPos = Pos;

        if(Node != ResearchRoot)
            Lines.Add({ ParentPos, Pos });

        CreateNodeAtPosition(Node, Pos);
        
        int32 NumNodes = Node->ChildNodes.Num();
        int32 Total = NumNodes * Size + (NumNodes - 1) * VPadding;
        int32 Y = -(Total / 2) + (Size / 2);

        Pos += FVector2D(Size + XPadding, Y);

        for (auto Child : Node->ChildNodes)
        {
            BuildTree(Child, InitialPos, Pos);
            Pos.Y += Size + VPadding;
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
    CanvasSlot->SetZOrder(2);
    CanvasSlot->SynchronizeProperties();

    ResearchNodeWidgets.Add(Widget);
}

void ULibraryWidget::ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node)
{
    UE_LOG(LogFrontier, Display, TEXT("Research tree changed"));

    if (ResearchRoot)
    {
        ResearchNodeWidgets.Empty();
        BuildTree(ResearchRoot, NodeStartPosition, NodeStartPosition);
        InitTree();
    }

    if (Type == EResearchTreeChangedType::NodeStateChanged && Node->State == EResearchState::Researched)
    {
        ShowResearchedPopup(Node);
    }
}

int32 ULibraryWidget::NativePaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled
) const
{
    if (bPaintLines)
    {
        const auto OffsetA = FVector2D(Size + LineThickness, Size / 2);
        const auto OffsetB = FVector2D(-LineThickness, Size / 2);

        for (const auto& Line : Lines)
        {
            TArray<FVector2D> Points;
            Points.Add(Line.A + OffsetA);
            Points.Add(Line.B + OffsetB);

            FSlateDrawElement::MakeLines(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry(),
                Points,
                ESlateDrawEffect::None,
                LineColour,
                true,
                LineThickness);
        }
    }

    return LayerId;
}
