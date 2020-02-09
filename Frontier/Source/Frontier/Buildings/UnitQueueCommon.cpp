// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "UnitQueueCommon.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

AUnitQueueCommon::AUnitQueueCommon() : Super()
{
    PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>("SpawnArea");
}

void AUnitQueueCommon::QueueUnit(TSubclassOf<AFrontierCharacter> Unit)
{
    auto DefaultUnit = Unit.GetDefaultObject();

    if (DefaultUnit->TrainedInBuilding == GetClass())
    {
        FUnitQueueItem Item;
        Item.Unit = Unit;
        Item.TimeRemaining = DefaultUnit->TrainTime;
		Item.SpawnLocation = (SpawnArea->GetRelativeTransform() * GetTransform()).GetLocation();

        Player->Resources -= DefaultUnit->Cost;
        UnitQueue.Push(Item);
        UnitQueueChangedEvent.Broadcast();
    }
}

void AUnitQueueCommon::RemoveQueuedUnit(int32 Index)
{
    if (UnitQueue.Num() > Index)
    {
        auto Item = UnitQueue[Index];
        Player->Resources += Item.Unit.GetDefaultObject()->Cost;

        UnitQueue.RemoveAt(Index);

        if(!IsRunningDedicatedServer())
            UnitQueueChangedEvent.Broadcast();
    }
}

void AUnitQueueCommon::OnRep_UnitQueue()
{
    UnitQueueChangedEvent.Broadcast();
}

void AUnitQueueCommon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (UnitQueue.Num() > 0)
    {
        FUnitQueueItem& Item = UnitQueue[0];
        Item.TimeRemaining -= DeltaTime;

        if (Item.TimeRemaining < 0.0f)
        {
            if (HasAuthority())
            {
                auto Transform = FTransform(Item.SpawnLocation);

                auto Unit = GetWorld()->SpawnActorDeferred<AFrontierCharacter>(
                    Item.Unit,
                    Transform,
                    Player,
                    (APawn*)nullptr,
                    ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
                );

				Unit->SetOwner(Player);
                Unit->Player = Player;

                UGameplayStatics::FinishSpawningActor(Unit, Transform);

                Player->Units.Add(Unit);
                UnitQueue.RemoveAt(0);

                if(!IsRunningDedicatedServer())
                    UnitQueueChangedEvent.Broadcast();
            }
        }
    }
}

bool AUnitQueueCommon::CanCreateUnit(TSubclassOf<AFrontierCharacter> Unit) const
{
    auto DefaultUnit = Unit.GetDefaultObject();

    return Player->Resources >= DefaultUnit->Cost && Player->IsObjectResearched(Unit);
}

void AUnitQueueCommon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AUnitQueueCommon, UnitQueue);
}