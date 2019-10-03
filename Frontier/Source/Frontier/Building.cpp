// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "UnrealNetwork.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Engine/World.h"

// Sets default values
ABuilding::ABuilding()
{
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        if (UnitQueue.Num() > 0)
        {
            FUnitQueueItem& Item = UnitQueue[0];
            Item.TimeRemaining -= DeltaTime;

            if (Item.TimeRemaining < 0.0f)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                auto Unit = GetWorld()->SpawnActor<AFrontierCharacter>(
                    Item.Unit,
                    Item.SpawnLocation + FVector(0.0f, 0.0f, 100.0f),
                    FRotator::ZeroRotator,
                    SpawnParams
                );

                Unit->Team = Player->Team;

                Player->Units.Add(Unit);
                UnitQueue.RemoveAt(0);
            }
        }
    }
}

void ABuilding::QueueUnit(TSubclassOf<AFrontierCharacter> Unit)
{
    if (HasAuthority())
    {
        FUnitQueueItem Item;
        Item.Unit = Unit;
        Item.TimeRemaining = Unit.GetDefaultObject()->TrainTime;
        Item.SpawnLocation = GetActorLocation();

        Player->Resources -= Unit.GetDefaultObject()->Cost;
        UnitQueue.Push(Item);
    }
}

void ABuilding::RemoveQueuedUnit(int32 Index)
{
    UnitQueue.RemoveAt(Index);
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuilding, Cost);
    DOREPLIFETIME(ABuilding, Player);
    DOREPLIFETIME(ABuilding, UnitQueue);
}

bool ABuilding::CanCreateUnit(TSubclassOf<AFrontierCharacter> Unit) const
{
    return Player->Resources >= Unit.GetDefaultObject()->Cost && Player->IsObjectResearched(Unit);
}
