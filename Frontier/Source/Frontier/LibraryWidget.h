// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBaseWidget.h"
#include "LibraryWidget.generated.h"

class UCanvasPanel;
class UResearchNode;
class UResearchNodeWidget;

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

private:
    void BuildTree(UResearchNode* Node, FVector2D Pos);
    void CreateNodeAtPosition(UResearchNode* Node, FVector2D Pos);

    UPROPERTY(Meta=(BindWidget))
    UCanvasPanel* Canvas;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UResearchNodeWidget> ResearchNodeWidgetClass;

    UPROPERTY(EditAnywhere)
    int Size = 200;

    UPROPERTY(EditAnywhere)
    int XSpacing = 200;

    UPROPERTY(EditAnywhere)
    int YSpacing = 200;

    UPROPERTY(EditAnywhere)
    int VPadding = 10;
};
