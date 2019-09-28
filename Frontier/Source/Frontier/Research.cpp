#include "Research.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Frontier.h"



UResearchNode* UResearchNode::AddChild(EResearchType InType, FResources InCost, TSubclassOf<AActor> InObject)
{
    auto Node = NewObject<UResearchNode>(GetOuter());
    Node->Type = InType;
    Node->Cost = InCost;
    Node->Object = InObject;

    ChildNodes.Add(Node);
    return Node;
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
    DOREPLIFETIME(UResearchNode, Object);
    DOREPLIFETIME(UResearchNode, bUnlocked);
    DOREPLIFETIME(UResearchNode, ChildNodes);
}