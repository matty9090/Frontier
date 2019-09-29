// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontierPlayerState.h"
#include "CoreMinimal.h"
#include "UnrealNetwork.h"
#include "Building.h"
#include "Engine/World.h"
#include "Engine/ActorChannel.h"
#include "FrontierCharacter.h"
#include "Research.h"

AFrontierPlayerState::AFrontierPlayerState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AFrontierPlayerState::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (HasAuthority())
    {
        ResearchRootNode = CreateResearchTree();
    }
}

UResearchNode* AFrontierPlayerState::CreateResearchTree_Implementation()
{
    return NewObject<UResearchNode>(this);
}

void AFrontierPlayerState::Tick(float DeltaTime)
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

                Unit->Team = Team;

                Units.Add(Unit);
                UnitQueue.RemoveAt(0);
            }
        }
    }
}

void AFrontierPlayerState::QueueUnit(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    if (HasAuthority() && Building->Team == Team)
    {
        FUnitQueueItem Item;
        Item.Unit = Unit;
        Item.TimeRemaining = Unit.GetDefaultObject()->TrainTime;
        Item.SpawnLocation = Building->GetActorLocation();

        Resources -= Unit.GetDefaultObject()->Cost;
        UnitQueue.Push(Item);
    }
}

void AFrontierPlayerState::UnlockResearchNode(UResearchNode* Node)
{
    if (HasAuthority())
    {
        Node->State = EResearchState::Available;
        OnResearchTreeChangedEvent.Broadcast();
    }
}

void AFrontierPlayerState::Research(UResearchNode* Node)
{
    if (HasAuthority())
    {
        Node->State = EResearchState::Researched;
        Node->Refresh();

        Resources -= Node->Cost;

        AvailableObjects.Append(Node->Objects);
        OnResearchTreeChangedEvent.Broadcast();
    }
}

bool AFrontierPlayerState::IsObjectResearched(TSubclassOf<AActor> Obj) const
{
    return AvailableObjects.Contains(Obj);
}

bool AFrontierPlayerState::CanCreateBuilding(TSubclassOf<ABuilding> Building) const
{
    return Resources >= Building.GetDefaultObject()->Cost && IsObjectResearched(Building);
}

bool AFrontierPlayerState::CanCreateUnit(TSubclassOf<AFrontierCharacter> Unit) const
{
    return Resources >= Unit.GetDefaultObject()->Cost && IsObjectResearched(Unit);
}

bool AFrontierPlayerState::CanResearchNode(UResearchNode* Node) const
{
    bool CanResearch = Resources >= Node->Cost && Node->State == EResearchState::Available;

    if (Node->Parent)
    {
        return CanResearch && Node->Parent->State == EResearchState::Researched;
    }

    return CanResearch;
}

void AFrontierPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerState, Resources);
    DOREPLIFETIME(AFrontierPlayerState, Team);
    DOREPLIFETIME(AFrontierPlayerState, Units);
    DOREPLIFETIME(AFrontierPlayerState, UnitQueue);
    DOREPLIFETIME(AFrontierPlayerState, MaxPopulation);
    DOREPLIFETIME(AFrontierPlayerState, ResearchRootNode);
    DOREPLIFETIME(AFrontierPlayerState, AvailableObjects);
}

bool AFrontierPlayerState::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool DidWrite = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    if (ResearchRootNode != nullptr)
    {
        DidWrite |= Channel->ReplicateSubobject(ResearchRootNode, *Bunch, *RepFlags);

        TArray<UResearchNode*> Nodes;
        ResearchRootNode->Traverse(Nodes);

        for (const auto& Node : Nodes)
        {
            DidWrite |= Channel->ReplicateSubobject(Node, *Bunch, *RepFlags);
        }
    }

    return DidWrite;
}