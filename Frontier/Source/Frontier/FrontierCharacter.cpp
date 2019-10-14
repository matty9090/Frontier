// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

AFrontierCharacter::AFrontierCharacter()
{
    // Set size for player capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AFrontierCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AFrontierCharacter::ShowOutline()
{
    GetMesh()->SetRenderCustomDepth(true);
}

void AFrontierCharacter::HideOutline()
{
    GetMesh()->SetRenderCustomDepth(false);
}

void AFrontierCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierCharacter, Player);
}