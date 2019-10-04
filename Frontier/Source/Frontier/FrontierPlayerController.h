// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerController.generated.h"

class ABuilding;
class UResearchNode;

UCLASS()
class AFrontierPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AFrontierPlayerController();

    void BeginPlay() override;
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    /** True if the controlled character should navigate to the mouse cursor. */
    uint32 bMoveToMouseCursor : 1;

    // Begin PlayerController interface
    virtual void PlayerTick(float DeltaTime) override;
    virtual void SetupInputComponent() override;
    // End PlayerController interface

    UFUNCTION(BlueprintImplementableEvent)
    void CreateUI();

    UFUNCTION(Client, Reliable)
    void ClientCreateUI();

    void OnRep_PlayerState() override;

    UPROPERTY(BlueprintReadWrite)
    ABuilding* HoveredBuilding = nullptr;

private:
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

    UFUNCTION()
    void OnRep_PlacedBuilding();

    UPROPERTY(ReplicatedUsing=OnRep_PlacedBuilding)
    ABuilding* PlacedBuilding = nullptr;
};
