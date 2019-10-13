// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "UnitQueueCommon.h"
#include "UnrealNetwork.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

AUnitQueueCommon::AUnitQueueCommon() : Super()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AUnitQueueCommon::QueueUnit(TSubclassOf<AFrontierCharacter> Unit)
{
    FUnitQueueItem Item;
    Item.Unit = Unit;
    Item.TimeRemaining = Unit.GetDefaultObject()->TrainTime;
    Item.SpawnLocation = GetActorLocation();

    Player->Resources -= Unit.GetDefaultObject()->Cost;
    UnitQueue.Push(Item);
    UnitQueueChangedEvent.Broadcast();
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
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                auto Unit = GetWorld()->SpawnActor<AFrontierCharacter>(
                    Item.Unit,
                    Item.SpawnLocation + FVector(0.0f, 0.0f, 100.0f),
                    FRotator::ZeroRotator,
                    SpawnParams
                );

				Unit->SetOwner(Player);
                Unit->Player = Player;

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
    return Player->Resources >= Unit.GetDefaultObject()->Cost && Player->IsObjectResearched(Unit);
}

void AUnitQueueCommon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AUnitQueueCommon, UnitQueue);
}