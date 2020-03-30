// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "DummyBuilding.h"
#include "Building.h"
#include "City.h"
#include "EngineUtils.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

// Sets default values
ADummyBuilding::ADummyBuilding()
{
    PrimaryActorTick.bCanEverTick = true;

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
    
    Requirements.Add(EBuildingPlacementConditions::AreaClear, FBuildingPlacementRequirement("Can't place here"));
    Requirements.Add(EBuildingPlacementConditions::CityNumLimit, FBuildingPlacementRequirement("Building limit reached"));
    Requirements.Add(EBuildingPlacementConditions::IsInCity, FBuildingPlacementRequirement("Outside city bounds"));
    Requirements.Add(EBuildingPlacementConditions::HaveResources, FBuildingPlacementRequirement("Not enough resources"));
    Requirements.Add(EBuildingPlacementConditions::IsResearched, FBuildingPlacementRequirement("Building not researched"));
    Requirements.Add(EBuildingPlacementConditions::NotInFog, FBuildingPlacementRequirement("Area not visible"));
}

void ADummyBuilding::BeginPlay()
{
    Super::BeginPlay();

    Requirements[EBuildingPlacementConditions::AreaClear].Met = true;

    if (IsValid(Player))
    {
        Requirements[EBuildingPlacementConditions::IsResearched].Met = Player->IsObjectResearched(BuildingType);
        Requirements[EBuildingPlacementConditions::HaveResources].Met = Player->Resources >= BuildingType.GetDefaultObject()->Cost;
    }

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
}

void ADummyBuilding::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    auto BoxExtent = Box->GetScaledBoxExtent();
    auto Extent = FMath::Max(BoxExtent.X, BoxExtent.Y);
    bool bCityNumLimit = false;
    bool bIsInCity = false;

    for (TActorIterator<ACity> Itr(GetWorld(), ACity::StaticClass()); Itr; ++Itr)
    {
        if ((*Itr)->CanPlaceBuilding(BuildingType))
        {
            bCityNumLimit = true;
        }

        if ((*Itr)->IsInCity(GetActorLocation(), Extent))
        {
            bIsInCity = true;
        }
    }

    Requirements[EBuildingPlacementConditions::CityNumLimit].Met = bCityNumLimit;
    Requirements[EBuildingPlacementConditions::IsInCity].Met = bIsInCity;

    if (Mesh)
    {
        auto NumMaterials = Mesh->GetMaterials().Num();

        for (int i = 0; i < NumMaterials; ++i)
            Mesh->SetMaterial(i, IsAllRequirementsMet() ? HoverMaterialGreen : HoverMaterialRed);
    }
}

bool ADummyBuilding::CanCreateBuilding() const
{
    return Requirements[EBuildingPlacementConditions::IsResearched]() &&
           Requirements[EBuildingPlacementConditions::HaveResources]();
}

bool ADummyBuilding::IsAllRequirementsMet() const
{
    for (auto Req : Requirements)
    {
        if (!Req.Value.Met)
        {
            return false;
        }
    }

    return true;
}

FString ADummyBuilding::GetPlacementErrorString() const
{
    for (auto Req : Requirements)
    {
        if (!Req.Value())
        {
            return Req.Value.Message;
        }
    }
    
    return "Can't place building";
}

void ADummyBuilding::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != this)
    {
        ++NumOverlapping;
        Requirements[EBuildingPlacementConditions::AreaClear].Met = false;
    }
}

void ADummyBuilding::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor != this)
    {
        --NumOverlapping;

        if (NumOverlapping <= 0)
        {
            Requirements[EBuildingPlacementConditions::AreaClear].Met = true;
        }
    }
}
