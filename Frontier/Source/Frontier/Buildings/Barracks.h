// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Barracks.generated.h"

class AFrontierCharacter;
class AFrontierPlayerState;

USTRUCT(BlueprintType)
struct FUnitQueueItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
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
class FRONTIER_API ABarracks : public ABuilding
{
	GENERATED_BODY()

public:
    ABarracks();

    virtual void Tick(float DeltaTime) override;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    bool CanCreateUnit(TSubclassOf<AFrontierCharacter> Unit) const;

    UFUNCTION(BlueprintCallable)
    void QueueUnit(TSubclassOf<AFrontierCharacter> Unit);

    UFUNCTION(BlueprintCallable)
    void RemoveQueuedUnit(int32 Index);

    DECLARE_EVENT(ABuilding, FUnitQueueChangedEvent);
    FUnitQueueChangedEvent& OnUnitQueueChanged() { return UnitQueueChangedEvent; }

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_UnitQueue)
    TArray<FUnitQueueItem> UnitQueue;

private:
    UFUNCTION()
    void OnRep_UnitQueue();

    FUnitQueueChangedEvent UnitQueueChangedEvent;
};
