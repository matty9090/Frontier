// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resources.h"

#include "Building.generated.h"

class AFrontierCharacter;
class AFrontierPlayerState;
class UBuildingBaseWidget;

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

UCLASS()
class FRONTIER_API ABuilding : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ABuilding();

    void QueueUnit(TSubclassOf<AFrontierCharacter> Unit);
    void RemoveQueuedUnit(int32 Index);

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    bool CanCreateUnit(TSubclassOf<AFrontierCharacter> Unit) const;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FResources Cost;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UBuildingBaseWidget> Widget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
    AFrontierPlayerState* Player;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<FUnitQueueItem> UnitQueue;
};
