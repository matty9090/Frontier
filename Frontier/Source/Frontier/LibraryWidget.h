// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBaseWidget.h"
#include "LibraryWidget.generated.h"

class UCanvasPanel;
class UResearchNode;
class UResearchNodeWidget;

USTRUCT()
struct FLine
{
    GENERATED_BODY()

    UPROPERTY()
    FVector2D A;

    UPROPERTY()
    FVector2D B;
};

/**
 * 
 */
UCLASS()
class FRONTIER_API ULibraryWidget : public UBuildingBaseWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct() override;

    UPROPERTY(BlueprintReadOnly)
    UResearchNode* ResearchRoot;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void InitTree();

    UPROPERTY(BlueprintReadWrite)
    TArray<UResearchNodeWidget*> ResearchNodeWidgets;

    UPROPERTY(BlueprintReadWrite)
    bool bPaintLines = false;

private:
    void BuildTree(UResearchNode* Node, FVector2D ParentPos, FVector2D Pos);
    void CreateNodeAtPosition(UResearchNode* Node, FVector2D Pos);
    void ResearchTreeChanged();
    
    int32 NativePaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled
    ) const override;

    UPROPERTY(Meta=(BindWidget))
    UCanvasPanel* Canvas;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UResearchNodeWidget> ResearchNodeWidgetClass;

    UPROPERTY(EditAnywhere)
    FVector2D NodeStartPosition = FVector2D(100, 500);

    UPROPERTY(EditAnywhere)
    FLinearColor LineColour;

    UPROPERTY(EditAnywhere)
    float LineThickness = 2.0f;

    UPROPERTY()
    TArray<FLine> Lines;

    UPROPERTY(EditAnywhere)
    int Size = 200;

    UPROPERTY(EditAnywhere)
    int XPadding = 40;

    UPROPERTY(EditAnywhere)
    int VPadding = 40;
};
