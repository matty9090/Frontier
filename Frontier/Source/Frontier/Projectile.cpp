// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/HealthComponent.h"
#include "FrontierPlayerState.h"

// Sets default values
AProjectile::AProjectile()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bReplicateMovement = true;
	PrimaryActorTick.bCanEverTick = true;

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
    MovementComponent->bRotationFollowsVelocity = true;
    MovementComponent->InitialSpeed = Speed;

    OnHitDelegate.BindUFunction(this, "OnHit");
    OnActorHit.Add(OnHitDelegate);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	auto Component = OtherActor->GetComponentByClass(UHealthComponent::StaticClass());
	if (Component != nullptr)
	{
		auto OtherActorHealth = Cast<UHealthComponent>(Component);
		if(OtherActorHealth->ReceiveDamage(Damage))
			Player->PlayerStats.UnitsKilled++;
	}
    Destroy();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

