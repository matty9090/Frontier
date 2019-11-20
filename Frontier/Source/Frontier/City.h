// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "City.generated.h"

class ABuilding;
class ATownHall;
class UWidgetComponent;
class AFrontierPlayerState;

UCLASS()
class FRONTIER_API ACity : public AActor
{
	GENERATED_BODY()
	
public:	
	ACity();
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    void AddBuilding(ABuilding* Building);
    void RemoveBuilding(ABuilding* Building);
    bool CanPlaceBuilding(TSubclassOf<ABuilding> Type, const FVector& DesiredPosition, float Bounds) const;
    void FinaliseCityPlayerWidgets();

    UPROPERTY(EditAnywhere)
    float Radius = 1600.0f;

	UPROPERTY(EditAnywhere)
	float FogRadius = 1800.0f;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* DecalMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UWidgetComponent* CityNameWidget = nullptr;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TArray<ABuilding*> Buildings;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Player)
    AFrontierPlayerState* Player = nullptr;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing=OnRep_CityName)
    FString CityName = "";

    UPROPERTY(EditAnywhere)
    UDecalComponent* CityRadiusDecal = nullptr;

protected:
	void BeginPlay() override;
    void EndPlay(const EEndPlayReason::Type Reason);

    UFUNCTION()
    void OnRep_Player();

    UFUNCTION()
    void OnRep_CityName();

    UFUNCTION(BlueprintImplementableEvent)
    void UpdateCityLabel();

private:
    UPROPERTY(EditAnywhere)
    int32 VillageNameMin = 1;

    UPROPERTY(EditAnywhere)
    int32 VillageNameMax = 6;

    FString GetRandomCityName() const;

    UPROPERTY(EditAnywhere)
    TMap<TSubclassOf<ABuilding>, int32> MaxBuildingNums;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ABuilding> MainBuildingClass;
};
