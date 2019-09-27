// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LibraryWidget.generated.h"

class UCanvasPanel;
class UResearchNode;

/**
 * 
 */
UCLASS()
class FRONTIER_API ULibraryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct() override;

    UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn))
    UResearchNode* ResearchRoot;

private:
    UPROPERTY(Meta=(BindWidget))
    UCanvasPanel* Canvas;
};
