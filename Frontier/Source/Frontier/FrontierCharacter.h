// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Resources.h"
#include "FrontierCharacter.generated.h"

UCLASS(Blueprintable)
class AFrontierCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFrontierCharacter();

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintImplementableEvent)
    void MoveToLocation(FVector Location, AActor* Object);

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

    UPROPERTY(BlueprintReadOnly, Meta=(ExposeOnSpawn))
    int32 Team = -1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FResources Cost;

private:

};

