// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontierPlayerState.h"
#include "CoreMinimal.h"
#include "UnrealNetwork.h"
#include "Building.h"
#include "Engine/World.h"
#include "FrontierCharacter.h"
#include "Research.h"

AFrontierPlayerState::AFrontierPlayerState()
{
    PrimaryActorTick.bCanEverTick = true;

    ResearchManager = CreateDefaultSubobject<UResearchManager>(TEXT("Research Manager"));
}

void AFrontierPlayerState::Tick(float DeltaTime)
{
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

                auto Unit = GetWorld()->SpawnActor<AFrontierCharacter>(Item.Unit, Item.SpawnLocation, FRotator::ZeroRotator, SpawnParams);
                Unit->Team = Team;

                Units.Add(Unit);
                UnitQueue.RemoveAt(0);
            }
        }
    }
}

void AFrontierPlayerState::QueueUnit(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    if (HasAuthority())
    {
        FUnitQueueItem Item;
        Item.Unit = Unit;
        Item.TimeRemaining = Unit.GetDefaultObject()->TrainTime;
        Item.SpawnLocation = Building->GetActorLocation();

        UnitQueue.Push(Item);
    }
}

void AFrontierPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerState, Resources);
    DOREPLIFETIME(AFrontierPlayerState, Team);
    DOREPLIFETIME(AFrontierPlayerState, Units);
    DOREPLIFETIME(AFrontierPlayerState, UnitQueue);
    DOREPLIFETIME(AFrontierPlayerState, MaxPopulation);
    DOREPLIFETIME(AFrontierPlayerState, ResearchManager);
}