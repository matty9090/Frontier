// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Research.h"
#include "Resources.h"
#include "FrontierHelperFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API UFrontierHelperFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintPure)
    static FResources CreateResources(int32 Wood = 0, int32 Stone = 0, int32 Metal = 0, int32 Gold = 0, int32 Food = 0, int32 Population = 0)
    {
        return FResources(Wood, Stone, Metal, Gold, Food, Population);
    }

    UFUNCTION(BlueprintCallable)
    static UResearchNode* CreateResearchNode(
        FString Name,
        FResources Cost,
        TArray<TSubclassOf<AActor>> Objects,
        EResearchState State = EResearchState::Locked,
        UResearchNode* Parent = nullptr,
        AActor* Outer = nullptr
    )
    {
        UResearchNode* Node = NewObject<UResearchNode>(Outer);
        Node->Name = Name;
        Node->State = State;
        Node->Cost = Cost;
        Node->Objects = Objects;
        Node->Parent = Parent;

        return Node;
    }
};
