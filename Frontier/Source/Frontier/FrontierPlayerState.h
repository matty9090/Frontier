// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"
#include "GameFramework/PlayerState.h"
#include "Resources.h"
#include "FrontierPlayerState.generated.h"

class ABuilding;
class AFrontierCharacter;
class UResearchNode;

DECLARE_MULTICAST_DELEGATE(FOnResearchTreeChangedEvent);

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
    int32 GetPopulation() const { return Units.Num(); }

    UFUNCTION(BlueprintCallable)
    void UnlockResearchNode(UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    void Research(UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    bool IsObjectResearched(TSubclassOf<AActor> Obj) const;

    UFUNCTION(BlueprintCallable)
    bool CanCreateBuilding(TSubclassOf<ABuilding> Building) const;

    UFUNCTION(BlueprintCallable)
    bool CanResearchNode(UResearchNode* Node) const;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    FResources Resources;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 Team;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 MaxPopulation = 40;

    UPROPERTY(Replicated, BlueprintReadOnly)
    UResearchNode* ResearchRootNode = nullptr;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<AFrontierCharacter*> Units;

    FOnResearchTreeChangedEvent OnResearchTreeChangedEvent;

private:
    UPROPERTY(Replicated)
    TArray<TSubclassOf<AActor>> AvailableObjects;
};