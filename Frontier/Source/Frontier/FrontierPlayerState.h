// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"
#include "GameFramework/PlayerState.h"
#include "Resources.h"
#include "FrontierPlayerState.generated.h"

class ABuilding;
class AFrontierCharacter;
class UResearchNode;

DECLARE_DELEGATE(FOnResearchTreeChangedEvent);

USTRUCT(BlueprintType)
struct FUnitQueueItem
{
    GENERATED_BODY()

    UPROPERTY()
    TSubclassOf<AFrontierCharacter> Unit;

    UPROPERTY(BlueprintReadOnly)
    float TimeRemaining;

    UPROPERTY(BlueprintReadOnly)
    FVector SpawnLocation;
};

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

    void QueueUnit(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building);
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    
    UFUNCTION(BlueprintCallable)
    int32 GetPopulation() const { return Units.Num(); }

    UFUNCTION(BlueprintCallable)
    void UnlockResearchNode(UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    void Research(UResearchNode* Node);

    UFUNCTION(BlueprintCallable)
    bool IsObjectResearched(TSubclassOf<AActor> Obj) const;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    FResources Resources;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 Team;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 MaxPopulation = 40;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<AFrontierCharacter*> Units;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<FUnitQueueItem> UnitQueue;

    UPROPERTY(Replicated, BlueprintReadOnly)
    UResearchNode* ResearchRootNode = nullptr;

    FOnResearchTreeChangedEvent OnResearchTreeChangedEvent;

private:
    UPROPERTY(Replicated)
    TArray<TSubclassOf<AActor>> AvailableObjects;
};