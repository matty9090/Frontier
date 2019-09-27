// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResearchNodeWidget.generated.h"

class UResearchNode;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FRONTIER_API UResearchNodeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct() override;

    UPROPERTY()
    UResearchNode* Node;

private:
    UPROPERTY(Meta=(BindWidget))
    UTextBlock* TxtName;
};
