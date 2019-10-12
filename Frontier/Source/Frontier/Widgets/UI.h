// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UUI : public UUserWidget
{
    GENERATED_BODY()
    
public:
    bool Initialize() override;

    UFUNCTION(BlueprintImplementableEvent)
    void ShowUI();

    UFUNCTION(BlueprintImplementableEvent)
    void HideUI();

    UPROPERTY(BlueprintReadWrite)
    bool bIsHidden = false;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void ResearchTreeChanged(EResearchTreeChangedType Type, UResearchNode* Node);

private:
    FDelegateHandle ResearchTreeChangedHandle;
};
