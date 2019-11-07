// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "City.h"
#include "FogOfWar.h"
#include "Buildings/Building.h"
#include "Buildings/TownHall.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

ACity::ACity()
{
    bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

    CityRadiusDecal = CreateDefaultSubobject<UDecalComponent>("CityRadiusDecal");
    CityRadiusDecal->DecalSize = FVector(16.0f, Radius, Radius);
    CityRadiusDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    CityRadiusDecal->SetDecalMaterial(DecalMaterial);

    RootComponent = CityRadiusDecal;
}

void ACity::BeginPlay()
{
	Super::BeginPlay();

    CityRadiusDecal->DecalSize = FVector(16.0f, Radius, Radius);
    CityRadiusDecal->MarkRenderStateDirty();

    if (HasAuthority())
    {
        auto BoxComponent = Cast<UBoxComponent>(MainBuildingClass.GetDefaultObject()->GetComponentByClass(UBoxComponent::StaticClass()));
        float Z = BoxComponent->GetScaledBoxExtent().Z;

        FTransform BuildingTransform(GetActorLocation() + FVector(0.0f, 0.0f, Z));
        
        ATownHall* StartBuilding = GetWorld()->SpawnActorDeferred<ATownHall>(MainBuildingClass, BuildingTransform, Player, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
        StartBuilding->Player = Player;
        StartBuilding->City   = this;
        StartBuilding->bBuilt = bInstantBuild;

        UGameplayStatics::FinishSpawningActor(StartBuilding, BuildingTransform);
        Buildings.Add(StartBuilding);
    }
}

void ACity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACity::AddBuilding(ABuilding* Building)
{
    Buildings.Add(Building);
}

void ACity::RemoveBuilding(ABuilding* Building)
{
    Buildings.Remove(Building);
}

bool ACity::CanPlaceBuilding(TSubclassOf<ABuilding> Type, const FVector& DesiredPosition, float Bounds) const
{
    int32 Total = 0;

    for (auto& Building : Buildings)
    {
        if (Building->GetClass() == Type)
        {
            ++Total;
        }
    }

    return Total < MaxBuildingNums[Type] &&
           FVector::Distance(DesiredPosition, GetActorLocation()) < Radius - Bounds;
}

