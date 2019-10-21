// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "PlusResourceCommon.h"
#include "ConstructorHelpers.h"
#include "Widgets/PlusResourceWidget.h"
#include "FrontierHelperFunctionLibrary.h"
#include "FrontierPlayerState.h"

APlusResourceCommon::APlusResourceCommon()
{
    PlusResourceWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlusResourceWidget"));
    PlusResourceWidget->SetupAttachment(RootComponent);
    PlusResourceWidget->SetWidgetSpace(EWidgetSpace::Screen);
    PlusResourceWidget->SetDrawAtDesiredSize(true);
    PlusResourceWidget->SetVisibility(false);

    static ConstructorHelpers::FClassFinder<UUserWidget> ResWidget(TEXT("/Game/Frontier/Blueprints/Widgets/WBP_PlusResources"));

    if (ResWidget.Succeeded())
    {
        PlusResourceWidget->SetWidgetClass(ResWidget.Class);
    }
}

void APlusResourceCommon::BeginPlay()
{
    Super::BeginPlay();

    PlusResourceWidget->SetVisibility(true);

    auto ResWidget = Cast<UPlusResourceWidget>(PlusResourceWidget->GetUserWidgetObject());
    ResWidget->Amount = Amount;
    ResWidget->Resource = UFrontierHelperFunctionLibrary::GetResourceName(ResourceType);

    Player->AddSpecificResources(Amount, ResourceType);
}

void APlusResourceCommon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Player->AddSpecificResources(Amount, ResourceType);
}
