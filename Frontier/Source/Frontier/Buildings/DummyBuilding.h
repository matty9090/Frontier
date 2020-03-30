// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DummyBuilding.generated.h"

class ABuilding;
class AFrontierPlayerState;

enum class EBuildingPlacementConditions
{
    HaveResources,
    IsInCity,
    CityNumLimit,
    AreaClear,
    IsResearched,
    NotInFog,
    _Max
};

struct FBuildingPlacementRequirement
{
    bool Met = false;
    const FString Message = "";

    bool operator ()() const { return Met; }
    FBuildingPlacementRequirement(FString Msg) : Message(Msg) {}
};

UCLASS()
class FRONTIER_API ADummyBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyBuilding();

    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    bool CanCreateBuilding() const;
    bool IsAllRequirementsMet() const;
    FString GetPlacementErrorString() const;

    UPROPERTY(EditAnywhere)
    UBoxComponent* Box = nullptr;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY()
    UMaterialInterface* HoverMaterialGreen = nullptr;

    UPROPERTY()
    UMaterialInterface* HoverMaterialRed = nullptr;

    UPROPERTY()
    TSubclassOf<ABuilding> BuildingType;

    AFrontierPlayerState* Player;
    TMap<EBuildingPlacementConditions, FBuildingPlacementRequirement> Requirements;

private:
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    int NumOverlapping = 0;

    FScriptDelegate BeginOverlapDelegate;
    FScriptDelegate EndOverlapDelegate;
};
