// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "DummyBuilding.h"
#include "Frontier.h"

// Sets default values
ADummyBuilding::ADummyBuilding()
{
    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);
    Box->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Box->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2); // Dummy
    Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

    RootComponent = Box;
    RootComponent->SetMobility(EComponentMobility::Movable);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Box);
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    BeginOverlapDelegate.BindUFunction(this, "OnBeginOverlap");
    EndOverlapDelegate.BindUFunction(this, "OnEndOverlap");

    Box->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
    Box->OnComponentEndOverlap.Add(EndOverlapDelegate);
}

void ADummyBuilding::BeginPlay()
{
    if (!HasAuthority())
    {
        SetActorHiddenInGame(GetOwner() == nullptr);
    }

    if (Mesh)
    {
        auto NumMaterials = Mesh->GetMaterials().Num();

        for (int i = 0; i < NumMaterials; ++i)
            Mesh->SetMaterial(i, HoverMaterialGreen);
    }

    bCanPlace = true;
}

void ADummyBuilding::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Mesh && OtherActor != this)
    {
        auto NumMaterials = Mesh->GetMaterials().Num();

        for (int i = 0; i < NumMaterials; ++i)
            Mesh->SetMaterial(i, HoverMaterialRed);

        ++NumOverlapping;
        bCanPlace = false;
    }
}

void ADummyBuilding::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Mesh && OtherActor != this)
    {
        --NumOverlapping;

        if (NumOverlapping <= 0)
        {
            auto NumMaterials = Mesh->GetMaterials().Num();

            for (int i = 0; i < NumMaterials; ++i)
                Mesh->SetMaterial(i, HoverMaterialGreen);

            bCanPlace = true;
        }
    }
}
