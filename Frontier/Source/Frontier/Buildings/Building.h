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

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere)
    UWidgetComponent* Tooltip = nullptr;

public:
    UPROPERTY(EditAnywhere)
    UBoxComponent* Box = nullptr;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh = nullptr;

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
    
    UPROPERTY(BlueprintReadWrite, Replicated, Meta=(ExposeOnSpawn))
    AFrontierPlayerState* Player;

private:
    UFUNCTION()
    void BeginMouseOver(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void EndMouseOver(UPrimitiveComponent* TouchedComponent);

    TScriptDelegate<> BeginMouseOverDelegate;
    TScriptDelegate<> EndMouseOverDelegate;
};
