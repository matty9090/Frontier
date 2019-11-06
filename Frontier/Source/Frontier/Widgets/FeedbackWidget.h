// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FeedbackWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UFeedbackWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly)
    FString Text;
};
