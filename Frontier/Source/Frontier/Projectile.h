// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UMeshComponent;
class UProjectileMovementComponent;
class AFrontierPlayerState;

UCLASS()
class FRONTIER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
    AProjectile();

    UPROPERTY(EditAnywhere)
    UProjectileMovementComponent* MovementComponent;

    UPROPERTY(EditAnywhere)
    float Speed = 3000.0f;

    UPROPERTY(EditAnywhere)
    int32 Damage = 20;

	AFrontierPlayerState* Player = nullptr;

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

public:	
	virtual void Tick(float DeltaTime) override;

private:
    FScriptDelegate OnHitDelegate;
};
