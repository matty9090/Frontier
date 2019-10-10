// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RoamPawn.generated.h"

UCLASS()
class FRONTIER_API ARoamPawn : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ARoamPawn();

    UFUNCTION(BlueprintImplementableEvent)
    float GetCameraHeight() const;

    UFUNCTION(BlueprintImplementableEvent)
    void SetCameraHeight(float Height);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
