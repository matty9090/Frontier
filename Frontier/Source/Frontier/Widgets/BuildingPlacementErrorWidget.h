// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingPlacementErrorWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UBuildingPlacementErrorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;
};
