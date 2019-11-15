// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resources.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TextureHolder.h"
#include "Building.generated.h"

class ACity;
class AFrontierCharacter;
class AFrontierPlayerState;
class UBuildingBaseWidget;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
    General     UMETA(DisplayName = "General"),
    Resource    UMETA(DisplayName = "Resource"),
    Combat      UMETA(DisplayName = "Combat")
};

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

	UFUNCTION(BlueprintCallable)
	bool IsConstructed() const { return bBuilt; }

	UFUNCTION(BlueprintCallable)
	bool Construct(float ConstructionAmount);

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(int Damage);

protected:
    virtual void BeginPlay() override;
    virtual void OnBuildingConstructed() {}

    UPROPERTY(EditAnywhere)
    UWidgetComponent* Tooltip = nullptr;

public:
    UPROPERTY(EditAnywhere)
    UBoxComponent* Box = nullptr;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* ConstructionMesh = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* BuildingMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
    FResources Cost;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UBuildingBaseWidget> Widget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BuildingName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString BuildingDesc;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FTextureHolder Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBuildingCategory BuildingCategory = EBuildingCategory::General;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	int32 HP;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Built)
    bool bBuilt = false;

    UPROPERTY()
    bool bRevealed = false;
    
    UPROPERTY(BlueprintReadWrite, Replicated, Meta=(ExposeOnSpawn))
    AFrontierPlayerState* Player;

    UPROPERTY(BlueprintReadOnly, Replicated)
    ACity* City;

private:
    UFUNCTION()
    void BeginMouseOver(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void EndMouseOver(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void OnRep_Built();

    TScriptDelegate<> BeginMouseOverDelegate;
    TScriptDelegate<> EndMouseOverDelegate;

	float ConstructionProgress = 0.f;
	const float MaxConstruct = 100.f;
};
