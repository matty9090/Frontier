// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DummyBuilding.generated.h"

UCLASS()
class FRONTIER_API ADummyBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyBuilding();

    void BeginPlay() override;

    UPROPERTY(EditAnywhere)
    UBoxComponent* Box = nullptr;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh = nullptr;
};
