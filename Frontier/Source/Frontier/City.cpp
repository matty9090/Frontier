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
    bAlwaysRelevant = true;
    bNetLoadOnClient = true;
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
    UE_LOG(LogFrontier, Verbose, TEXT("City begin play"));

    if (HasAuthority())
    {
        CityName = GetRandomCityName();
        UE_LOG(LogFrontier, Display, TEXT("Generating city: %s"), *CityName);
    }

    CityRadiusDecal->DecalSize = FVector(300.0f, Radius, Radius);
    CityRadiusDecal->MarkRenderStateDirty();

    UE_LOG(LogFrontier, Display, TEXT("Set widget visibility %s, %s"), *CityName, *GetName());

    if (Player)
    {
        Player->Cities.Add(this);
    }
    else
    {
        UE_LOG(LogFrontier, Warning, TEXT("City: Null player state!"));
    }

	Super::BeginPlay();
}

void ACity::EndPlay(const EEndPlayReason::Type Reason)
{
    Player->Cities.Remove(this);
}

void ACity::OnRep_Player()
{
    FinaliseCityPlayerWidgets();
}

void ACity::OnRep_CityName()
{
    UpdateCityLabel();
}

FString ACity::GetRandomCityName() const
{
    FString S = "BCDFGHJKLMNPQRSTVWXYZ";
    FString L = "AEIOU";
    FString C = "bcdfghjklmnpqrstvwxyz";
    FString V = "aeiou";

    FString PX[4] = { "ton", "ville", "forth", "ford" };

    int Len = FMath::RandRange(VillageNameMin, VillageNameMax);
    FString Out = "";

    bool Sv = FMath::RandBool();

    if (Sv == true)
        Out += S[FMath::Rand() % 21];
    else
        Out += L[FMath::Rand() % 5];

    for (int i = 0; i < Len; i++) {
        Sv = !Sv;

        if (Sv == true)
            Out += C[FMath::Rand() % 21];
        else
            Out += V[FMath::Rand() % 5];
    }

    Out += PX[FMath::Rand() % 4];

    return Out;
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

void ACity::FinaliseCityPlayerWidgets()
{
    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (FrontierController)
    {
        if (FrontierController->PlayerState)
        {
            if (Player->Team == FrontierController->GetPlayerState<AFrontierPlayerState>()->Team)
            {
                FrontierController->FogOfWar->RevealCircle(GetActorLocation(), Radius);
                CityNameWidget->SetVisibility(true);
                CityRadiusDecal->SetVisibility(true);
            }
            else
            {
                CityNameWidget->SetVisibility(false);
                CityRadiusDecal->SetVisibility(false);
            }
        }
        else
        {
            UE_LOG(LogFrontier, Display, TEXT("TownHall: player state or city is null!"));
        }
    }
    else
    {
        UE_LOG(LogFrontier, Display, TEXT("TownHall: Controller is null!"));
    }
}

void ACity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACity, Player);
    DOREPLIFETIME(ACity, CityName);
    DOREPLIFETIME(ACity, Buildings);
}