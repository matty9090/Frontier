// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "ResourceMultiplierCommon.h"
#include "ConstructorHelpers.h"
#include "Widgets/FeedbackWidget.h"
#include "FrontierHelperFunctionLibrary.h"
#include "FrontierPlayerState.h"

AResourceMultiplierCommon::AResourceMultiplierCommon()
{
    ResMultiplierWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ResMultiplierWidget"));
    ResMultiplierWidget->SetupAttachment(RootComponent);
    ResMultiplierWidget->SetWidgetSpace(EWidgetSpace::Screen);
    ResMultiplierWidget->SetDrawAtDesiredSize(true);

    static ConstructorHelpers::FClassFinder<UUserWidget> ResWidget(TEXT("/Game/Frontier/Blueprints/Widgets/WBP_GenericFeedback"));

    if (ResWidget.Succeeded())
    {
        ResMultiplierWidget->SetWidgetClass(ResWidget.Class);
    }
}

void AResourceMultiplierCommon::BeginPlay()
{
    Super::BeginPlay();

    auto ResWidget = Cast<UFeedbackWidget>(ResMultiplierWidget->GetUserWidgetObject());
    ResWidget->Text  = "<Green>+" + FString::FromInt(PercentIncrease) + "%</> ";
    ResWidget->Text += UFrontierHelperFunctionLibrary::GetResourceName(ResourceType);
    ResWidget->Text += " production";
}

void AResourceMultiplierCommon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Player->AddResourceMultiplier(-PercentIncrease / 100.0f, ResourceType);
}

void AResourceMultiplierCommon::OnBuildingConstructed()
{
    Cast<UFeedbackWidget>(ResMultiplierWidget->GetUserWidgetObject())->Play();
    Player->AddResourceMultiplier(PercentIncrease / 100.0f, ResourceType);
}