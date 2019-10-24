// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "Building.h"
#include "UnrealNetwork.h"
#include "FogOfWar.h"
#include "ConstructorHelpers.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "FrontierGameState.h"
#include "FrontierPlayerController.h"
#include "Widgets/BuildingBaseWidget.h"
#include "Widgets/PlusResourceWidget.h"
#include "FrontierHelperFunctionLibrary.h"

// Sets default values
ABuilding::ABuilding() : Super()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bNetLoadOnClient = true;
    PrimaryActorTick.bCanEverTick = false;

    BeginMouseOverDelegate.BindUFunction(this, "BeginMouseOver");
    EndMouseOverDelegate.BindUFunction(this, "EndMouseOver");

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);
    Box->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
    Box->CanCharacterStepUpOn = ECB_No;
    
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
    Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
    
    Box->OnBeginCursorOver.Add(BeginMouseOverDelegate);
    Box->OnEndCursorOver.Add(EndMouseOverDelegate);

    RootComponent = Box;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Box);

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

    Widget = UBuildingBaseWidget::StaticClass();
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

    if (GetOwner())
    {
        auto Controller = Cast<AFrontierPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        
        if (Controller)
        {
            auto GS = Cast<AFrontierGameState>(UGameplayStatics::GetGameState(GetWorld()));
            Controller->FogOfWar->RevealCircle(GetActorLocation(), GS->FowRevealRadius);
        }
    }
}

void ABuilding::BeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
    if(Player->GetOwner() == GetWorld()->GetFirstPlayerController())
        Tooltip->SetVisibility(true);
}

void ABuilding::EndMouseOver(UPrimitiveComponent* TouchedComponent)
{
    Tooltip->SetVisibility(false);
}

void ABuilding::ShowOutline()
{
    Mesh->SetRenderCustomDepth(true);
}

void ABuilding::HideOutline()
{
    Mesh->SetRenderCustomDepth(false);
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuilding, Cost);
    DOREPLIFETIME(ABuilding, Player);
}
