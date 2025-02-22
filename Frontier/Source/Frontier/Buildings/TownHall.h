// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "Buildings/UnitQueueCommon.h"
#include "TownHall.generated.h"

class ACity;
class USphereComponent;

UCLASS()
class FRONTIER_API ATownHall : public AUnitQueueCommon
{
	GENERATED_BODY()
	
public:
    ATownHall();

    void Tick(float DeltaTime) override;

protected:
    void OnBuildingConstructed() override;
    void OnBuildingDestroyed() override;

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<ACity> CityClass;
};
