// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resources.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Building.generated.h"

class AFrontierCharacter;
class AFrontierPlayerState;
class UBuildingBaseWidget;
class UStaticMeshComponent;

UCLASS()
class FRONTIER_API ABuilding : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ABuilding();

    UFUNCTION(BlueprintCallable)
    void ShowOutline();

    UFUNCTION(BlueprintCallable)
    void HideOutline();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh = nullptr;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere)
    UBoxComponent* Box = nullptr;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Outline = nullptr;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
    FResources Cost;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UBuildingBaseWidget> Widget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontier Object")
    FString BuildingName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontier Object")
    FString BuildingDesc;
    
    UPROPERTY(BlueprintReadWrite, Replicated, Meta=(ExposeOnSpawn))
    AFrontierPlayerState* Player;
};
