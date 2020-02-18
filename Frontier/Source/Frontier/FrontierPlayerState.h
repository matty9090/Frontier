// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "DelegateCombinations.h"
#include "Containers/Queue.h"
#include "GameFramework/Actor.h"
#include "SubclassOf.h"
#include "GameFramework/PlayerState.h"
#include "Resources.h"
#include "Research.h"
#include "FrontierPlayerState.generated.h"

class ACity;
class ABuilding;
class AFrontierCharacter;
class UResearchNode;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnResearchTreeChangedEvent, EResearchTreeChangedType, UResearchNode*);
/**
 * 
 */
UCLASS()
class FRONTIER_API AFrontierPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AFrontierPlayerState();

    void PostInitializeComponents() override;
    void Tick(float DeltaTime) override;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    
    UFUNCTION(BlueprintNativeEvent)
    UResearchNode* CreateResearchTree();

    UFUNCTION(BlueprintCallable)
    void AddResources(FResources Res);

    UFUNCTION(BlueprintCallable)
	void AddResourceMultiplier(float ResMultiplier, EResource Type);

	UFUNCTION(BlueprintCallable)
	void AddSpecificResources(float Res, EResource Type);

    // Do we need this?
    UFUNCTION(BlueprintCallable)
    int32 GetPopulation() const { return Units.Num(); }

    UFUNCTION(BlueprintCallable)
    void UnlockResearchNode(UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    void Research(UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    bool IsObjectResearched(TSubclassOf<AActor> Obj) const;

    UFUNCTION(BlueprintCallable)
    bool CanCreateBuilding(TSubclassOf<ABuilding> Building, const FVector& Position) const;

    UFUNCTION(BlueprintCallable)
    bool CanCreateBuildingUI(TSubclassOf<ABuilding> Building) const;

    UFUNCTION(BlueprintCallable)
    bool CanResearchNode(UResearchNode* Node) const;

    UFUNCTION(BlueprintCallable)
    TArray<TSubclassOf<ABuilding>> GetResearchedBuildings() const;

	void RemoveCity(ACity* city);

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    FResources Resources;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    FResources ResourceMultiplier;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 Team;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 MaxPopulation = 40;

    UPROPERTY(Replicated, BlueprintReadOnly)
    UResearchNode* ResearchRootNode = nullptr;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<AFrontierCharacter*> Units;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<ACity*> Cities;

    FOnResearchTreeChangedEvent OnResearchTreeChangedEvent;

private:
    UPROPERTY(Replicated)
    TArray<TSubclassOf<AActor>> AvailableObjects;
};