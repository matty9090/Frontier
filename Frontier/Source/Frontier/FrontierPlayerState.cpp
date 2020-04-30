// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierPlayerState.h"
#include "CoreMinimal.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "FrontierGameMode.h"
#include "City.h"
#include "FrontierCharacter.h"
#include "Frontier.h"

AFrontierPlayerState::AFrontierPlayerState()
    : ResourceMultiplier(1, 1, 1, 1, 1, 1, 1)
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
    Resources += Res * ResourceMultiplier;
	PlayerStats.TotalResources += Res * ResourceMultiplier;
}

void AFrontierPlayerState::AddResourceMultiplier(float ResMultiplier, EResource Type)
{
    ResourceMultiplier.Resources[Type] += ResMultiplier;
}

float AFrontierPlayerState::AddSpecificResources(float Res, EResource Type)
{
    float Multiplied = Res * ResourceMultiplier.Resources[Type];
	Resources.Resources[Type] += Multiplied;
	PlayerStats.TotalResources.Resources[Type] += Res * ResourceMultiplier.Resources[Type];
    return Multiplied;
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

bool AFrontierPlayerState::CanCreateBuilding(TSubclassOf<ABuilding> Building, const FVector& Position) const
{
    bool CanPlaceInCity = false;
    auto BoxComponent = Cast<UBoxComponent>(Building.GetDefaultObject()->GetComponentByClass(UBoxComponent::StaticClass()));
    auto BoxExtent = BoxComponent->GetScaledBoxExtent();
    auto Extent = FMath::Max(BoxExtent.X, BoxExtent.Y);

    for (auto& City : Cities)
    {
        if (City->CanPlaceBuilding(Building) && City->IsInCity(Position, Extent))
        {
            CanPlaceInCity = true;
            break;
        }
    }

    return Resources >= Building.GetDefaultObject()->Cost && IsObjectResearched(Building) && CanPlaceInCity;
}

bool AFrontierPlayerState::CanCreateBuildingUI(TSubclassOf<ABuilding> Building) const
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

TArray<TSubclassOf<ABuilding>> AFrontierPlayerState::GetResearchedBuildings() const
{
    TArray<TSubclassOf<ABuilding>> Buildings;

    for (auto Actor : AvailableObjects)
    {
        if (Actor->IsChildOf(ABuilding::StaticClass()))
            Buildings.Add(TSubclassOf<ABuilding>(*Actor));
    }

    return Buildings;
}

void AFrontierPlayerState::RemoveCity(ACity* city)
{
	Cities.Remove(city);

	if (Cities.Num() <= 0 && HasAuthority())
	{
		Cast<AFrontierGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GameOver();
	}
}

void AFrontierPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerState, Resources);
    DOREPLIFETIME(AFrontierPlayerState, ResourceMultiplier);
    DOREPLIFETIME(AFrontierPlayerState, Team);
    DOREPLIFETIME(AFrontierPlayerState, Units);
    DOREPLIFETIME(AFrontierPlayerState, Cities);
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