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
    void SetCanPlace(bool bInCanPlace);
    bool CanPlace() const { return bCanPlace && !bIsOverlapping; }

    UPROPERTY(EditAnywhere)
    UBoxComponent* Box = nullptr;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY()
    UMaterialInterface* HoverMaterialGreen = nullptr;

    UPROPERTY()
    UMaterialInterface* HoverMaterialRed = nullptr;

private:
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    int NumOverlapping = 0;

    FScriptDelegate BeginOverlapDelegate;
    FScriptDelegate EndOverlapDelegate;

    UPROPERTY()
    bool bCanPlace = false;

    UPROPERTY()
    bool bIsOverlapping = false;
};
