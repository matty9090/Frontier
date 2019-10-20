// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "ResourceMultiplierCommon.h"
#include "ConstructorHelpers.h"
#include "Widgets/MultiplyResourceWidget.h"
#include "FrontierHelperFunctionLibrary.h"
#include "FrontierPlayerState.h"

AResourceMultiplierCommon::AResourceMultiplierCommon()
{
    ResMultiplierWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ResMultiplierWidget"));
    ResMultiplierWidget->SetupAttachment(RootComponent);
    ResMultiplierWidget->SetWidgetSpace(EWidgetSpace::Screen);
    ResMultiplierWidget->SetDrawAtDesiredSize(true);
    ResMultiplierWidget->SetVisibility(false);

    static ConstructorHelpers::FClassFinder<UUserWidget> ResWidget(TEXT("/Game/Frontier/Blueprints/Widgets/WBP_MultiplyResources"));

    if (ResWidget.Succeeded())
    {
        ResMultiplierWidget->SetWidgetClass(ResWidget.Class);
    }
}

void AResourceMultiplierCommon::BeginPlay()
{
    Super::BeginPlay();

    ResMultiplierWidget->SetVisibility(true);

    auto ResWidget = Cast<UMultiplyResourceWidget>(ResMultiplierWidget->GetUserWidgetObject());
    ResWidget->Multiplier = PercentIncrease;
    ResWidget->Resource = UFrontierHelperFunctionLibrary::GetResourceName(ResourceType);

    Player->AddResourceMultiplier(PercentIncrease / 100.0f, ResourceType);
}

void AResourceMultiplierCommon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Player->AddResourceMultiplier(-PercentIncrease / 100.0f, ResourceType);
}
