// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "Projectile.h"
#include "Turret.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class FRONTIER_API ATurret : public ABuilding
{
	GENERATED_BODY()
	
public:
    ATurret();

    void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USphereComponent* FireLocation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AProjectile> ProjectileClass = AProjectile::StaticClass();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Range = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float FireRate = 1.0f;

private:
    void FindTarget();

    UPROPERTY()
    AActor* CurrentTarget = nullptr;

    bool bCanFire = true;
    float Timer = 0.0f;
};
