// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Research.h"
#include "Resources.h"
#include "Buildings/Building.h"
#include "FrontierCharacter.h"
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

    UFUNCTION(BlueprintCallable)
    static FString TryGetFrontierObjectName(TSubclassOf<AActor> Object)
    {
        auto Obj = Object.GetDefaultObject();

        if (!Obj) return "";
        
        auto Building = Cast<ABuilding>(Obj);
        if (Building) return Building->BuildingName;

        auto Unit = Cast<AFrontierCharacter>(Obj);
        if (Unit) return Unit->UnitName;

        return "";
    }

    UFUNCTION(BlueprintCallable)
    static FString TryGetFrontierObjectDescription(TSubclassOf<AActor> Object)
    {
        auto Obj = Object.GetDefaultObject();

        if (!Obj) return "";
        
        auto Building = Cast<ABuilding>(Obj);
        if (Building) return Building->BuildingDesc;

        auto Unit = Cast<AFrontierCharacter>(Obj);
        if (Unit) return Unit->UnitDesc;

        return "";
    }

    UFUNCTION(BlueprintPure)
    static FString GetResourcesAvailableString(FResources Target, FResources Current)
    {
        auto Available = Current.GetResourcesAvailable(Target);

        auto GetTxt = [Available, Target, Current](EResource Resource) {
            return FString::FromInt(Target.Resources[Resource])
                 + FString(Available[Resource] ? " <img id=\"Tick\"/>" : "<img id=\"Cross\"/>");
        };

        FString Str = "Wood " + GetTxt(EResource::Wood);
        Str += "\nStone " + GetTxt(EResource::Stone);
        Str += "\nMetal " + GetTxt(EResource::Metal);
        Str += "\nGold " + GetTxt(EResource::Gold);
        Str += "\nFood " + GetTxt(EResource::Food);
        Str += "\nPopulation " + GetTxt(EResource::Population);

        return Str;
    }
};
