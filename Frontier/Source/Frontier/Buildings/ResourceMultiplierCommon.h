// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "WidgetComponent.h"
#include "Resources.h"
#include "ResourceMultiplierCommon.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API AResourceMultiplierCommon : public ABuilding
{
	GENERATED_BODY()

public:
    AResourceMultiplierCommon();

protected:
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere)
    UWidgetComponent* ResMultiplierWidget = nullptr;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EResource ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PercentIncrease = 20.0f;
};
