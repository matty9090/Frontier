// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "PlusResourceCommon.h"
#include "ConstructorHelpers.h"
#include "Widgets/FeedbackWidget.h"
#include "FrontierHelperFunctionLibrary.h"
#include "FrontierPlayerState.h"

APlusResourceCommon::APlusResourceCommon()
{
    PlusResourceWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlusResourceWidget"));
    PlusResourceWidget->SetupAttachment(RootComponent);
    PlusResourceWidget->SetWidgetSpace(EWidgetSpace::Screen);
    PlusResourceWidget->SetDrawAtDesiredSize(true);

    static ConstructorHelpers::FClassFinder<UUserWidget> ResWidget(TEXT("/Game/Frontier/Blueprints/Widgets/WBP_GenericFeedback"));

    if (ResWidget.Succeeded())
    {
        PlusResourceWidget->SetWidgetClass(ResWidget.Class);
    }
}

void APlusResourceCommon::BeginPlay()
{
    Super::BeginPlay();

    auto ResWidget = Cast<UFeedbackWidget>(PlusResourceWidget->GetUserWidgetObject());
    ResWidget->Text  = "<Green>+" + FString::FromInt(Amount) + "</> ";
    ResWidget->Text += UFrontierHelperFunctionLibrary::GetResourceName(ResourceType);

    Player->AddSpecificResources(Amount, ResourceType);
}

void APlusResourceCommon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    Player->AddSpecificResources(Amount, ResourceType);
}

void APlusResourceCommon::OnBuildingConstructed()
{
    Cast<UFeedbackWidget>(PlusResourceWidget->GetUserWidgetObject())->Play();
}