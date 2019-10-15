// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "Turret.h"
#include "EngineUtils.h"
#include "Components/SphereComponent.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "FrontierHelperFunctionLibrary.h"
#include "DrawDebugHelpers.h"

ATurret::ATurret() : Timer(FireRate)
{
    PrimaryActorTick.bCanEverTick = true;

    FireLocation = CreateDefaultSubobject<USphereComponent>(TEXT("Fire Location"));
    FireLocation->SetHiddenInGame(true);
    FireLocation->SetSphereRadius(4.0f);
}

void ATurret::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Timer -= DeltaTime;

    if (Timer < 0.0f)
    {
        Timer = FireRate;
        bCanFire = true;
    }

    if (HasAuthority())
    {
        FindTarget();

        if (CurrentTarget && bCanFire)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            FVector FirePoint = GetActorLocation() + FireLocation->GetRelativeTransform().GetLocation();
            auto Rotator = (CurrentTarget->GetActorLocation() - FirePoint).Rotation();

            GetWorld()->SpawnActor<AProjectile>(
                ProjectileClass,
                FirePoint,
                Rotator,
                SpawnParams
            );

            bCanFire = false;
            Timer = FireRate;
        }
    }
}

void ATurret::FindTarget()
{
    CurrentTarget = nullptr;

    if (!Player)
        return;

    TArray<AActor*> EnemyUnits;
    
    for (TObjectIterator<AFrontierCharacter> It; It; ++It)
    {
        if((*It)->Player->Team != Player->Team && !(*It)->IsPendingKill())
            EnemyUnits.Add(*It);
    }

    auto Closest = UFrontierHelperFunctionLibrary::GetClosestObject(GetActorLocation(), EnemyUnits);

    if (Closest && FVector::Distance(Closest->GetActorLocation(), GetActorLocation()) < Range)
    {
        CurrentTarget = Closest;
    }
}
