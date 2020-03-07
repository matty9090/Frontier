// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "ResourcesContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UResourcesContainerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(Meta=(BindWidget))
    UTextBlock* Label;

    UPROPERTY(Meta=(BindWidget))
    UTextBlock* Amount;
};
