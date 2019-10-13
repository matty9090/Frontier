// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "FrontierPlayerState.h"
#include "CoreMinimal.h"
#include "UnrealNetwork.h"
#include "Buildings/Building.h"
#include "Engine/World.h"
#include "Engine/ActorChannel.h"
#include "FrontierCharacter.h"
#include "Frontier.h"

AFrontierPlayerState::AFrontierPlayerState()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AFrontierPlayerState::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (HasAuthority())
    {
        ResearchRootNode = CreateResearchTree();
        Research(ResearchRootNode);
    }
}

UResearchNode* AFrontierPlayerState::CreateResearchTree_Implementation()
{
    return NewObject<UResearchNode>(this);
}

void AFrontierPlayerState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);    
}

void AFrontierPlayerState::AddResources(FResources Res)
{
    Resources += Res;
}

void AFrontierPlayerState::AddSpecificResources(int Res, EResource Type)
{
	Resources.Resources[Type] += Res;
}

void AFrontierPlayerState::UnlockResearchNode(UResearchNode* Node)
{
    Node->State = EResearchState::Available;
    OnResearchTreeChangedEvent.Broadcast(EResearchTreeChangedType::NodeStateChanged, Node);
}

void AFrontierPlayerState::Research(UResearchNode* Node)
{
    Node->State = EResearchState::Researched;
    Node->Refresh();

    Resources -= Node->Cost;

    AvailableObjects.Append(Node->Objects);
    OnResearchTreeChangedEvent.Broadcast(EResearchTreeChangedType::NodeStateChanged, Node);
}

bool AFrontierPlayerState::IsObjectResearched(TSubclassOf<AActor> Obj) const
{
    return AvailableObjects.Contains(Obj);
}

bool AFrontierPlayerState::CanCreateBuilding(TSubclassOf<ABuilding> Building) const
{
    return Resources >= Building.GetDefaultObject()->Cost && IsObjectResearched(Building);
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