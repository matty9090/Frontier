// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "TownHall.h"
#include "City.h"
#include "FogOfWar.h"
#include "Frontier.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"

ATownHall::ATownHall()
{

}

void ATownHall::OnBuildingConstructed()
{
    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (FrontierController)
    {
        FrontierController->GetCityBuiltEvent().Broadcast(City);

        if (City && FrontierController->PlayerState && Player->Team == Cast<AFrontierPlayerState>(FrontierController->PlayerState)->Team)
        {
            FrontierController->FogOfWar->RevealCircle(GetActorLocation(), City->Radius);
        }
    }
}