// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "House.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API AHouse : public ABuilding
{
    GENERATED_BODY()
	
protected:
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int Population = 20;
};
