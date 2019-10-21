// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlusResourceWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UPlusResourceWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly)
    int32 Amount;

    UPROPERTY(BlueprintReadOnly)
    FString Resource;
};
