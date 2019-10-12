// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "Building.h"
#include "UnrealNetwork.h"
#include "ConstructorHelpers.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"

// Sets default values
ABuilding::ABuilding() : Super()
{
    PrimaryActorTick.bCanEverTick = false;

    BeginMouseOverDelegate.BindUFunction(this, "BeginMouseOver");
    EndMouseOverDelegate.BindUFunction(this, "EndMouseOver");

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);
    Box->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    Box->OnBeginCursorOver.Add(BeginMouseOverDelegate);
    Box->OnEndCursorOver.Add(EndMouseOverDelegate);

    RootComponent = Box;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Box);

    Outline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Outline"));
    Outline->SetupAttachment(Mesh);
    Outline->SetVisibility(false);

    Tooltip = CreateDefaultSubobject<UWidgetComponent>(TEXT("Tooltip"));
    Tooltip->SetupAttachment(RootComponent);
    Tooltip->AddRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
    Tooltip->SetWidgetSpace(EWidgetSpace::Screen);
    Tooltip->SetDrawAtDesiredSize(true);
    Tooltip->SetVisibility(false);

    static ConstructorHelpers::FClassFinder<UUserWidget> TooltipClass(TEXT("/Game/Frontier/Blueprints/Widgets/Tooltips/WBP_Tooltip_BuildingHover"));

    if (TooltipClass.Succeeded())
    {
        Tooltip->SetWidgetClass(TooltipClass.Class);
    }
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
    Super::BeginPlay();

    auto TooltipWidget = Tooltip->GetUserWidgetObject();

    if (TooltipWidget)
    {
        auto Property = FindField<UTextProperty>(TooltipWidget->GetClass(), TEXT("BuildingName"));

        if (Property)
        {
            Property->SetPropertyValue_InContainer(TooltipWidget, FText::FromString(BuildingName));
        }
    }
}

void ABuilding::BeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
    Tooltip->SetVisibility(true);
}

void ABuilding::EndMouseOver(UPrimitiveComponent* TouchedComponent)
{
    Tooltip->SetVisibility(false);
}

void ABuilding::ShowOutline()
{
    Outline->SetVisibility(true);
}

void ABuilding::HideOutline()
{
    Outline->SetVisibility(false);
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuilding, Cost);
    DOREPLIFETIME(ABuilding, Player);
}
