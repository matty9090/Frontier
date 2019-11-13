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
    FTransform CityTransform(GetActorLocation());
    City = GetWorld()->SpawnActorDeferred<ACity>(CityClass, CityTransform, Player, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    City->Player = Player;
    UGameplayStatics::FinishSpawningActor(City, CityTransform);

    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (FrontierController)
    {
        FrontierController->GetCityBuiltEvent().Broadcast(City);

        if (City && FrontierController->PlayerState && Player->Team == Cast<AFrontierPlayerState>(FrontierController->PlayerState)->Team)
        {
            FrontierController->FogOfWar->RevealCircle(GetActorLocation(), City->Radius);
        }
    }

    City->AddBuilding(this);
}