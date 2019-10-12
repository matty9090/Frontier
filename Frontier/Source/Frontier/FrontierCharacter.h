// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Resources.h"
#include "FrontierCharacter.generated.h"

class AFrontierPlayerState;

UCLASS(Blueprintable)
class AFrontierCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFrontierCharacter();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintImplementableEvent)
    void ShowOutline();

    UFUNCTION(BlueprintImplementableEvent)
    void HideOutline();

    UFUNCTION(BlueprintImplementableEvent)
    void MoveToLocation(FVector Location, AActor* Object);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontier Object")
    FString UnitName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontier Object")
    FString UnitDesc;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WalkSpeed = 500;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Armour = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Health = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float AttackStrength = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool CanGather = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GatherSpeed = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float TrainTime = 1;

    UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn), Replicated)
    AFrontierPlayerState* Player = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FResources Cost;

private:

};

