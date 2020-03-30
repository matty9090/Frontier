// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Research.h"
#include "Resources.h"
#include "BaseResource.h"
#include "Buildings/Building.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
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

    UFUNCTION(BlueprintCallable)
    static AFrontierPlayerState* TryGetFrontierObjectPlayer(AActor* Obj)
    {
        if (!Obj) return nullptr;
        
        auto Building = Cast<ABuilding>(Obj);
        if (Building) return Building->Player;

        auto Unit = Cast<AFrontierCharacter>(Obj);
        if (Unit) return Unit->Player;

        return nullptr;
    }

    UFUNCTION(BlueprintPure)
    static FString GetResourcesAvailableString(FResources Target, FResources Current)
    {
        auto Available = Current.GetResourcesAvailable(Target);

        auto GetTxt = [Available, Target, Current](EResource Resource) {
            return FString(Available[Resource] ? "<Green>" : "<Red>")
                 + FString::FromInt(Target.Resources[Resource])
                 + FString("</>");
        };

        FString Str = "Wood " + GetTxt(EResource::Wood);
        Str += "\nStone " + GetTxt(EResource::Stone);
        Str += "\nMetal " + GetTxt(EResource::Metal);
        Str += "\nGold " + GetTxt(EResource::Gold);
        Str += "\nFood " + GetTxt(EResource::Food);
        Str += "\nPopulation " + GetTxt(EResource::Population);

        return Str;
    }

    UFUNCTION(BlueprintPure)
    static FString GetResourceName(EResource Res);

	UFUNCTION(BlueprintPure)
	static ABuilding* GetClosestBuilding(FVector Position, AFrontierPlayerState* Player, TArray<ABuilding*> Objects, bool bConstructed = true)
	{
        ABuilding* Closest = nullptr;
		float Distance = MAX_FLT;

		for (auto Actor : Objects)
		{
			float TmpDistance = FVector::DistSquared(Position, Actor->GetActorLocation());

            if (TmpDistance < Distance && Player == Actor->Player && bConstructed == Actor->bBuilt)
			{
				Distance = TmpDistance;
				Closest = Actor;
			}
		}

		return Closest;
	}

    UFUNCTION(BlueprintPure)
	static ABaseResource* GetClosestResource(FVector Position, AFrontierPlayerState* Player, TArray<ABaseResource*> Objects)
	{
        ABaseResource* Closest = nullptr;
		float Distance = MAX_FLT;

		for (auto Actor : Objects)
		{
			float TmpDistance = FVector::DistSquared(Position, Actor->GetActorLocation());

			if (TmpDistance < Distance)
			{
				Distance = TmpDistance;
				Closest = Actor;
			}
		}

		return Closest;
	}

    UFUNCTION(BlueprintPure)
	static AFrontierCharacter* GetClosestEnemy(FVector Position, AFrontierPlayerState* Player, TArray<AFrontierCharacter*> Characters)
	{
        AFrontierCharacter* Closest = nullptr;
		float Distance = MAX_FLT;

		for (auto Actor : Characters)
		{
			float TmpDistance = FVector::DistSquared(Position, Actor->GetActorLocation());

			if (TmpDistance < Distance && Player->Team != Actor->Player->Team)
			{
				Distance = TmpDistance;
				Closest = Actor;
			}
		}

		return Closest;
	}

	UFUNCTION(BlueprintPure)
	static FRotator LookAt(FVector Position, FVector Target)
	{
		FRotator r = UKismetMathLibrary::FindLookAtRotation(Position, Target);
		return FRotator(0, r.Yaw, 0);
	}
};
