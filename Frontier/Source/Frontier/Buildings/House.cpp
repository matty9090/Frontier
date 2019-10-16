// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "House.h"
#include "FrontierPlayerState.h"
#include "Frontier.h"
#include "Widgets/PlusResourceWidget.h"
#include "FrontierHelperFunctionLibrary.h"

void AHouse::BeginPlay()
{
    Super::BeginPlay();

    PlusResource->SetVisibility(true);

    auto PlusResWidget = Cast<UPlusResourceWidget>(PlusResource->GetUserWidgetObject());
    PlusResWidget->Amount = Population;
    PlusResWidget->Resource = UFrontierHelperFunctionLibrary::GetResourceName(EResource::MaxPop);

    if (Player)
        Player->AddSpecificResources(Population, EResource::MaxPop);
}

void AHouse::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (Player)
        Player->AddSpecificResources(-Population, EResource::MaxPop);
}
