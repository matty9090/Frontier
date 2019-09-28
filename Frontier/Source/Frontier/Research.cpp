#include "Research.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

UResearchNode* UResearchNode::AddChild(EResearchType InType, FResources InCost, TSubclassOf<AActor> InObject)
{
    auto Node = NewObject<UResearchNode>(GetOuter());
    Node->Parent = this;
    Node->Type = InType;
    Node->Cost = InCost;
    Node->Object = InObject;

    ChildNodes.Add(Node);
    return Node;
}

void UResearchNode::OnRep_State()
{
    auto PS = Cast<AFrontierPlayerState>(GetOuter());

    if (PS)
    {
        PS->OnResearchTreeChangedEvent.ExecuteIfBound();
    }
}

void UResearchNode::Refresh()
{
    if (Parent && State != EResearchState::Researched)
    {
        if (Parent->State == EResearchState::Researched)
        {
            State = EResearchState::Available;
        }
    }

    for (auto& Node : ChildNodes)
    {
        if (Node)
        {
            Node->Refresh();
        }
    }
}

void UResearchNode::Traverse(TArray<UResearchNode*>& OutNodes)
{
    for (const auto& Node : ChildNodes)
    {
        if (Node)
        {
            OutNodes.Add(Node);
            Node->Traverse(OutNodes);
        }
    }
}

void UResearchNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UResearchNode, Name);
    DOREPLIFETIME(UResearchNode, Type);
    DOREPLIFETIME(UResearchNode, Cost);
    DOREPLIFETIME(UResearchNode, State);
    DOREPLIFETIME(UResearchNode, Object);
    DOREPLIFETIME(UResearchNode, Parent);
    DOREPLIFETIME(UResearchNode, ChildNodes);
}