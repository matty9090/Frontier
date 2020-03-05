// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Resources.h"
#include "TextureHolder.h"
#include "BaseResource.h"
#include "AIController.h"
#include "Projectile.h"
#include "Components/UnitTypeComponent.h"
#include "FrontierCharacter.generated.h"

class ABuilding;
class AFrontierPlayerState;
class UHealthComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class ECharacterStates : uint8
{
	Idle  UMETA(DisplayName = "Idle"),
	Harvesting  UMETA(DisplayName = "Harvesting"),
	Attacking  UMETA(DisplayName = "Attacking"),
	Building  UMETA(DisplayName = "Building"),
	Moving  UMETA(DisplayName = "Moving"),
	Depositing  UMETA(DisplayName = "Depositing")
};

UCLASS(Blueprintable)
class AFrontierCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFrontierCharacter();

	void PostInitializeComponents() override;
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

    UFUNCTION(BlueprintCallable)
    void ShowOutline();

    UFUNCTION(BlueprintCallable)
    void HideOutline();

	//Move
    UFUNCTION()
    void MoveToLocation(FVector Location, AActor* Object);

	UFUNCTION(BlueprintCallable)
	void MoveTo(AActor* Actor);

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void UpdateNav();

	//states
	UFUNCTION(BlueprintCallable)
	void StateIdle();

	UFUNCTION(BlueprintCallable)
	void StateHarvest();

	UFUNCTION(BlueprintCallable)
	void StateAttacking();

	UFUNCTION(BlueprintCallable)
	void StateBuilding();

	UFUNCTION(BlueprintCallable)
	void StateMoving();

	//harvesting
	UFUNCTION(BlueprintCallable)
	void SetHarvest();

	UFUNCTION(BlueprintCallable)
	void FinishDeposit();

	UFUNCTION(BlueprintCallable)
	void HarvestResource();

	UFUNCTION(BlueprintCallable)
	void FindNewHarvest();

	//build functions
	UFUNCTION(BlueprintCallable)
	void SetBuild();

	UFUNCTION(BlueprintCallable)
	void Construct();

	//attack functions
	UFUNCTION(BlueprintCallable)
	void SetAttack();

	UFUNCTION(BlueprintCallable)
	void Attack();

	//pause timers
	UFUNCTION(BlueprintCallable)
	void PauseTimerFunctions();

	UPROPERTY()
	FTimerHandle AttackTimerHandler;

	UPROPERTY()
	FTimerHandle ConstructTimerHandler;

	UPROPERTY()
	FTimerHandle HarvestTimerHandler;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString UnitName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString UnitDesc;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ABuilding> TrainedInBuilding;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WalkSpeed = 500;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Armour = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float AttackStrength = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveRange = 70;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCanGather = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsArcher = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AProjectile> ProjectileClass = AProjectile::StaticClass();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GatherSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BuildSpeed = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float TrainTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeldResourceLimit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FTextureHolder Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USceneComponent* LaunchPosition;

    UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn), Replicated)
    AFrontierPlayerState* Player = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FResources Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UUnitTypeComponent* UnitTypeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* HealthBar;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	ECharacterStates State;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	AActor* MoveObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	ABaseResource* HarvestObject;

	UPROPERTY(BlueprintReadOnly, Replicated)
	UClass* LastHarvestClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeldResources;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EResource HeldResourceType;

	UPROPERTY(BlueprintReadOnly)
	bool bRevealed = false;

private:
    FScriptDelegate ActorHitDelegate;
	FScriptDelegate MoveCompleteDelegate;
};