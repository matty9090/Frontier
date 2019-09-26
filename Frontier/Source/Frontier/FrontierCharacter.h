// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
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

    UPROPERTY(EditAnywhere)
    float WalkSpeed;

    UPROPERTY(EditAnywhere)
    float Armour;

    UPROPERTY(EditAnywhere)
    float Health;

    UPROPERTY(EditAnywhere)
    float AttackStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CanGather;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GatherSpeed;

	UPROPERTY(BlueprintReadOnly, Meta=(ExposeOnSpawn))
	int32 Team;

private:

};

