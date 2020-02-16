// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "TownHall.h"
#include "City.h"
#include "FogOfWar.h"
#include "Frontier.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "Kismet/GameplayStatics.h"

ATownHall::ATownHall()
{

}

void ATownHall::OnBuildingConstructed()
{
    UE_LOG(LogFrontier, Verbose, TEXT("Town hall constructed"));

    if (HasAuthority())
    {
        FTransform CityTransform(GetActorLocation());
        City = GetWorld()->SpawnActorDeferred<ACity>(CityClass, CityTransform, Player, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
        City->Player = Player;
        City->AddBuilding(this);
        UGameplayStatics::FinishSpawningActor(City, CityTransform);
    }

    if (City)
    {
        City->FinaliseCityPlayerWidgets();
    }
}

void ATownHall::EndPlay(const EEndPlayReason::Type Reason)
{
    if (IsValid(Player) && IsValid(City))
    {
        Player->RemoveCity(City);

        if (City)
        {
            City->Destroy();
        }
    }
}
