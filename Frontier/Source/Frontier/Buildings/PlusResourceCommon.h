// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "WidgetComponent.h"
#include "Resources.h"
#include "PlusResourceCommon.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API APlusResourceCommon : public ABuilding
{
	GENERATED_BODY()

public:
    APlusResourceCommon();

protected:
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere)
    UWidgetComponent* PlusResourceWidget = nullptr;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EResource ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Amount = 1.0f;
};
