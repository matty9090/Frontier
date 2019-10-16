// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "DummyBuilding.h"

// Sets default values
ADummyBuilding::ADummyBuilding()
{
    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);
    Box->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

    RootComponent = Box;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Box);

    RootComponent->SetMobility(EComponentMobility::Movable);
    SetActorEnableCollision(false);
}

void ADummyBuilding::BeginPlay()
{
    if (!HasAuthority())
    {
        SetActorHiddenInGame(GetOwner() == nullptr);
    }
}
