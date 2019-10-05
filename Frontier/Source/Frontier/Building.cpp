// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "UnrealNetwork.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"

// Sets default values
ABuilding::ABuilding()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);
    Box->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Box);

    Outline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Outline"));
    Outline->SetupAttachment(Mesh);
    Outline->SetVisibility(false);

    RootComponent = Box;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
    Super::BeginPlay();
}

void ABuilding::ShowOutline()
{
    Outline->SetVisibility(true);
}

void ABuilding::HideOutline()
{
    Outline->SetVisibility(false);
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuilding, Cost);
    DOREPLIFETIME(ABuilding, Player);
}