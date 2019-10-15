// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "House.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"

void AHouse::BeginPlay()
{
    if (Player)
        Player->AddSpecificResources(Population, EResource::MaxPop);
}

void AHouse::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    if (Player)
        Player->AddSpecificResources(-Population, EResource::MaxPop);
}
