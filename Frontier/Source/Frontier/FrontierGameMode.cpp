// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierGameMode.h"
#include "FrontierPlayerController.h"
#include "FrontierPlayerState.h"
#include "FrontierCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Buildings/Building.h"

AFrontierGameMode::AFrontierGameMode()
{
    
}

void AFrontierGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{    
    Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    auto PS = Cast<AFrontierPlayerState>(NewPlayer->PlayerState);

    auto Location = StartSpot->GetActorLocation();
    Location.Z = 0;

    auto BoxComponent = Cast<UBoxComponent>(StartBuildingClass.GetDefaultObject()->GetComponentByClass(UBoxComponent::StaticClass()));
    float Z = BoxComponent->GetScaledBoxExtent().Z;

    auto Library = GetWorld()->SpawnActor<ABuilding>(StartBuildingClass, Location + FVector(0.0f, 0.0f, Z), FRotator::ZeroRotator, SpawnParams);
    Library->Player = PS;

    auto Worker = GetWorld()->SpawnActor<AFrontierCharacter>(WorkerClass, Location + FVector(300.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
    Worker->Player = PS;
}
