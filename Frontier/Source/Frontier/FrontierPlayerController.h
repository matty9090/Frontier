// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontierCharacter.h"
#include "WidgetAnimation.h"
#include "FrontierPlayerController.generated.h"

class UUI;
class ACity;
class AFogOfWar;
class ABuilding;
class UResearchNode;
class ADummyBuilding;
class UBuildingBaseWidget;
class UResourcesContainerWidget;
class UBuildingPlacementErrorWidget;

DECLARE_DELEGATE(FBuildingConstructedEvent)
DECLARE_DELEGATE_OneParam(FPlayerKilledEvent, AFrontierCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FCityBuiltEvent, ACity*);

UENUM(BlueprintType)
enum class EControllerState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    SelectedUnit        UMETA(DisplayName = "SelectedObject"),
    SelectedBuilding    UMETA(DisplayName = "SelectedBuilding"),
    PlacingBuilding     UMETA(DisplayName = "PlacingBuilding")
};

UENUM(BlueprintType)
enum class ECursorState : uint8
{
    Default             UMETA(DisplayName = "Default"),
    Build               UMETA(DisplayName = "Build"),
    Attack              UMETA(DisplayName = "Attack"),
    Send                UMETA(DisplayName = "Send"),
    Harvest             UMETA(DisplayName = "Harvest")
};

UENUM(BlueprintType)
enum class ESound : uint8
{
    Default             UMETA(DisplayName = "Default"),
    Error               UMETA(DisplayName = "Error"),
    Place               UMETA(DisplayName = "Place"),
    Rotate              UMETA(DisplayName = "Rotate"),
    Click               UMETA(DisplayName = "Click"),
    Select              UMETA(DisplayName = "Select"),
    Built               UMETA(DisplayName = "Built"),
    ActionSent          UMETA(DisplayName = "ActionSent"),
    ActionBuild         UMETA(DisplayName = "ActionBuild"),
    ActionHarvest       UMETA(DisplayName = "ActionHarvest"),
    ActionClickBuilding UMETA(DisplayName = "ActionClickBuilding"),
    _Max
};

UCLASS(Blueprintable)
class AFrontierPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AFrontierPlayerController(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(Client, Reliable)
    void OnGameOver(const TArray<FPlayerStats>& Stats, FUniqueNetIdRepl WinningPlayer);

    void BeginPlay() override;
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    FCityBuiltEvent GetCityBuiltEvent() { return CityBuiltEvent; }
    FBuildingConstructedEvent GetBuildingConstructedEvent() { return BuildingConstructedEvent; }

    UFUNCTION(BlueprintCallable)
    void SetHoveredBuilding(TSubclassOf<ABuilding> BuildingType);

    UPROPERTY()
    AFogOfWar* FogOfWar;

    UPROPERTY(BlueprintReadOnly)
    ECursorState CursorState = ECursorState::Default;

    UPROPERTY(BlueprintReadOnly)
    UTextureRenderTarget2D* MinimapTarget;

    FPlayerKilledEvent PlayerKilledEvent;

protected:
    /** True if the controlled character should navigate to the mouse cursor. */
    uint32 bMoveToMouseCursor : 1;

    // Begin PlayerController interface
    void PlayerTick(float DeltaTime) override;
    void SetupInputComponent() override;
    // End PlayerController interface

private:
    UFUNCTION()
    void BuildingUIAnimationFinished();

    void OnRep_PlayerState() override;

    UFUNCTION()
    void OnRep_PlacedBuilding();

    void ShowResourceContainsWidget(ABaseResource* Res, int MX, int MY);

    // Begin Input
    void OnMoveUp(float Value);
    void OnMoveRight(float Value);
    void OnZoom(float Value);
    void OnSelectDown();
    void OnSelectUp();
    void OnSend();
    void OnRotate();
    // End Input

    // Begin RPCs
    UFUNCTION(Client, Reliable)
    void ClientCreateUI();

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerSpawnBuilding(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
    void ServerMoveAIToLocation(const TArray<AFrontierCharacter*>& AI, FVector Location, AActor* Object);

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
    float MouseMovementPadding = 1.0f;

    UPROPERTY(EditAnywhere)
    float FormationSeparation = 50.0f;

    UPROPERTY(EditAnywhere)
    UCurveFloat* MovementCurve = nullptr;

    UPROPERTY(EditAnywhere)
    UCurveFloat* ZoomCurve = nullptr;

    UPROPERTY(EditAnywhere)
    float MinimapSize = 20400.0f;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUI> UIClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UResourcesContainerWidget> ResourcesContainerClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UBuildingPlacementErrorWidget> BuildingPlacementErrorClass;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* HoverMaterialGreen = nullptr;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* HoverMaterialRed = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* MovementMaterialGreen = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* MovementMaterialRed = nullptr;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AFogOfWar> FogOfWarClass;

    UPROPERTY(EditAnywhere)
    TMap<ESound, USoundBase*> UserSounds;

    UPROPERTY()
    TMap<ESound, UAudioComponent*> Sounds;

    UPROPERTY()
    UUI* UI;

    UPROPERTY()
    EControllerState ControllerState = EControllerState::Idle;

    UPROPERTY()
    ADummyBuilding* HoveredBuilding = nullptr;

    UPROPERTY()
    TSubclassOf<ABuilding> HoveredBuildingType = nullptr;

    UPROPERTY(ReplicatedUsing=OnRep_PlacedBuilding)
    ABuilding* PlacedBuilding = nullptr;

    UPROPERTY()
    TArray<AFrontierCharacter*> SelectedUnits;

    UPROPERTY()
    ABuilding* SelectedBuilding = nullptr;

    UPROPERTY()
    UBuildingBaseWidget* SelectedBuildingUI = nullptr;

    UPROPERTY()
    FWidgetAnimationDynamicEvent AnimationFinishedEvent;

    UPROPERTY()
    UResourcesContainerWidget* ResourcesContainerWidget;
    
    UPROPERTY()
    UBuildingPlacementErrorWidget* BuildingPlacementErrorWidget;

    FCityBuiltEvent CityBuiltEvent;
    FBuildingConstructedEvent BuildingConstructedEvent;
};
