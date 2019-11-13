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

    UPROPERTY(EditAnywhere)
    float Radius = 1600.0f;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* DecalMaterial = nullptr;

    UPROPERTY(EditAnywhere)
    UWidgetComponent* CityNameWidget = nullptr;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TArray<ABuilding*> Buildings;

    UPROPERTY(BlueprintReadOnly, Replicated)
    AFrontierPlayerState* Player;

    UPROPERTY(BlueprintReadWrite)
    FString CityName;

    UPROPERTY(EditAnywhere)
    UDecalComponent* CityRadiusDecal = nullptr;

protected:
	void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere)
    TMap<TSubclassOf<ABuilding>, int32> MaxBuildingNums;

    UPROPERTY(EditAnywhere)
    TSubclassOf<ABuilding> MainBuildingClass;
};
