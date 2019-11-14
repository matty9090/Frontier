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

    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (FrontierController)
    {
        if (City && FrontierController->PlayerState)
        {
            if (Player->Team == Cast<AFrontierPlayerState>(FrontierController->PlayerState)->Team)
            {
                FrontierController->FogOfWar->RevealCircle(GetActorLocation(), City->Radius);
            }
            else
            {
                City->CityNameWidget->SetVisibility(false);
                City->CityRadiusDecal->SetVisibility(false);
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

void ATownHall::EndPlay(const EEndPlayReason::Type Reason)
{
    if (City)
    {
        City->Destroy();
    }
}
