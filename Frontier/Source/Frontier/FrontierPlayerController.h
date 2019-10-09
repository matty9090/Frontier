// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontierCharacter.h"
#include "WidgetAnimation.h"
#include "FrontierPlayerController.generated.h"

class UUI;
class ABuilding;
class UResearchNode;
class UBuildingBaseWidget;

UENUM(BlueprintType)
enum class EControllerState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    SelectedObject      UMETA(DisplayName = "SelectedObject"),
    PlacingBuilding     UMETA(DisplayName = "PlacingBuilding")
};

UCLASS(Blueprintable)
class AFrontierPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AFrontierPlayerController();

    void BeginPlay() override;
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    void SetHoveredBuilding(TSubclassOf<ABuilding> BuildingType);

protected:
    /** True if the controlled character should navigate to the mouse cursor. */
    uint32 bMoveToMouseCursor : 1;

    // Begin PlayerController interface
    virtual void PlayerTick(float DeltaTime) override;
    virtual void SetupInputComponent() override;
    // End PlayerController interface

private:
    UFUNCTION()
    void CreateUI();

    UFUNCTION()
    void BuildingUIAnimationFinished();

    void OnRep_PlayerState() override;

    UFUNCTION()
    void OnRep_PlacedBuilding();

    // Begin Input
    void OnMoveUp(float Value);
    void OnMoveRight(float Value);
    void OnSelect();
    void OnSend();
    void OnZoom(float Value);
    // End Input

    // Begin RPCs
    UFUNCTION(Client, Reliable)
    void ClientCreateUI();

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerSpawnBuilding(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerMoveAIToLocation(AFrontierCharacter* AI, FVector Location, AActor* Object);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerQueueUnit(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerRemoveQueuedUnit(int Index, ABuilding* Building);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerResearch(UResearchNode* Node);
    // End RPCs

    UPROPERTY(EditAnywhere)
    float MoveSpeed = 8.0f;

    UPROPERTY(EditAnywhere)
    float ZoomSpeed = 50.0f;

    UPROPERTY(EditAnywhere)
    float MinZoom = 40.0f;

    UPROPERTY(EditAnywhere)
    float MaxZoom = 4400.0f;

    UPROPERTY(EditAnywhere)
    float MouseMovementPadding = 52.0f;

    UPROPERTY(EditAnywhere)
    UCurveFloat* MovementCurve = nullptr;

    UPROPERTY(EditAnywhere)
    UCurveFloat* ZoomCurve = nullptr;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUI> UIClass;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* HoverMaterialGreen = nullptr;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* HoverMaterialRed = nullptr;

    UPROPERTY()
    UUI* UI;

    UPROPERTY()
    EControllerState ControllerState = EControllerState::Idle;

    UPROPERTY()
    ABuilding* HoveredBuilding = nullptr;

    UPROPERTY(ReplicatedUsing=OnRep_PlacedBuilding)
    ABuilding* PlacedBuilding = nullptr;

    UPROPERTY()
    AFrontierCharacter* SelectedUnit = nullptr;

    UPROPERTY()
    ABuilding* SelectedBuilding = nullptr;

    UPROPERTY()
    UBuildingBaseWidget* SelectedBuildingUI = nullptr;

    UPROPERTY()
    FWidgetAnimationDynamicEvent AnimationFinishedEvent;
};
