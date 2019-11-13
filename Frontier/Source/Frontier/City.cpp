// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "City.h"
#include "FogOfWar.h"
#include "UnrealNetwork.h"
#include "Buildings/Building.h"
#include "Buildings/TownHall.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Frontier.h"

ACity::ACity()
{
    bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

    CityRadiusDecal = CreateDefaultSubobject<UDecalComponent>("CityRadiusDecal");
    CityRadiusDecal->DecalSize = FVector(300.0f, Radius, Radius);
    CityRadiusDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    CityRadiusDecal->SetDecalMaterial(DecalMaterial);

    RootComponent = CityRadiusDecal;

    CityNameWidget = CreateDefaultSubobject<UWidgetComponent>("CityNameWidget");
    CityNameWidget->SetupAttachment(RootComponent);
    CityNameWidget->SetWidgetSpace(EWidgetSpace::Screen);
    CityNameWidget->SetDrawAtDesiredSize(true);
}

void ACity::BeginPlay()
{
	Super::BeginPlay();

    CityRadiusDecal->DecalSize = FVector(300.0f, Radius, Radius);
    CityRadiusDecal->MarkRenderStateDirty();
    CityRadiusDecal->SetVisibility(true);
    CityNameWidget->SetVisibility(true);

    Player->Cities.Add(this);
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

void ACity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACity, Player);
    DOREPLIFETIME(ACity, Buildings);
}