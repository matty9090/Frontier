// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Resources.h"
#include "TextureHolder.h"
#include "FrontierCharacter.generated.h"

class ABuilding;
class AFrontierPlayerState;
class UHealthComponent;
class UWidgetComponent;

UCLASS(Blueprintable)
class AFrontierCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFrontierCharacter();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

	void BeginPlay() override;

    UFUNCTION()
    void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION(BlueprintCallable)
    void ShowOutline();

    UFUNCTION(BlueprintCallable)
    void HideOutline();

    UFUNCTION(BlueprintImplementableEvent)
    void MoveToLocation(FVector Location, AActor* Object);

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(int Damage);

    UFUNCTION()
    void OnRep_Health();

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Health)
    float Health = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float AttackStrength = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool CanGather = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GatherSpeed = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float TrainTime = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FTextureHolder Icon;

    UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn), Replicated)
    AFrontierPlayerState* Player = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FResources Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* HealthBar;
private:
    FScriptDelegate ActorHitDelegate;
};