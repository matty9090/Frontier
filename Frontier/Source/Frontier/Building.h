// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resources.h"

#include "Building.generated.h"

class UBuildingBaseWidget;

UCLASS()
class FRONTIER_API ABuilding : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ABuilding();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
    int32 Team;
};
