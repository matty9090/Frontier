// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "TownHall.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class FRONTIER_API ATownHall : public ABuilding
{
	GENERATED_BODY()
	
public:
    ATownHall();

protected:
    void OnBuildingConstructed() override;

private:

};
