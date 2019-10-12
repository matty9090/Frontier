// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

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
    void UpdateNodeProperties();

    UPROPERTY(BlueprintReadOnly)
    UResearchNode* Node;

private:
    UPROPERTY(Meta=(BindWidget))
    UTextBlock* TxtName;
};
