// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplyResourceWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UMultiplyResourceWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly)
    float Multiplier;

    UPROPERTY(BlueprintReadOnly)
    FString Resource;
};
