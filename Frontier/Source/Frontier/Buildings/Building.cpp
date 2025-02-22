// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "Building.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "City.h"
#include "FogOfWar.h"
#include "ConstructorHelpers.h"
#include "FrontierGameMode.h"
#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/RevealFogComponent.h"
#include "Components/FlamePositionComponent.h"
#include "Widgets/HealthBarWidget.h"
#include "FrontierGameState.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "Widgets/BuildingBaseWidget.h"
#include "FrontierHelperFunctionLibrary.h"
#include "Frontier.h"

// Sets default values
ABuilding::ABuilding() 
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bNetLoadOnClient = true;
    PrimaryActorTick.bCanEverTick = true;

    BeginMouseOverDelegate.BindUFunction(this, "BeginMouseOver");
    EndMouseOverDelegate.BindUFunction(this, "EndMouseOver");

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);
    Box->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
    Box->CanCharacterStepUpOn = ECB_Yes;
    
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

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);
	HealthBar->SetDrawAtDesiredSize(true);
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->AddRelativeLocation(FVector(0.0f, 0.0f, 150.0f));

    Tooltip = CreateDefaultSubobject<UWidgetComponent>(TEXT("Tooltip"));
    Tooltip->SetupAttachment(RootComponent);
    Tooltip->AddRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
    Tooltip->SetWidgetSpace(EWidgetSpace::Screen);
    Tooltip->SetDrawAtDesiredSize(true);
    Tooltip->SetVisibility(false);

    RevealFogComponent = CreateDefaultSubobject<URevealFogComponent>(TEXT("RevealFog"));
    FireSound = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSound"));
    FireSound->SetAutoActivate(false);
    FireSound->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UUserWidget> BarClass(TEXT("/Game/Frontier/Blueprints/Widgets/WBP_HealthBar"));

	if (BarClass.Succeeded())
	{
		HealthBar->SetWidgetClass(BarClass.Class);
	}

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

	Mesh->SetStaticMesh(bBuilt ? BuildingMesh : ConstructionMesh);
    auto TooltipWidget = Tooltip->GetUserWidgetObject();

    if (TooltipWidget)
    {
        auto Property = FindField<UTextProperty>(TooltipWidget->GetClass(), TEXT("BuildingName"));

        if (Property)
        {
            Property->SetPropertyValue_InContainer(TooltipWidget, FText::FromString(BuildingName));
        }
    }

    if (bBuilt)
    {
        OnBuildingConstructed();
    }

    auto FireLocations = GetComponentsByClass(UFlamePositionComponent::StaticClass());

    for (auto Location : FireLocations)
    {
        USceneComponent* SceneLocation = Cast<USceneComponent>(Location);
        auto ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, SceneLocation->GetComponentLocation(), FRotator::ZeroRotator,false);
        ParticleSystem->DeactivateSystem();
        FireParticleSystems.Add(ParticleSystem);
    }

	HealthComponent->HealthChangeEvent.AddLambda([&](AActor* Actor, float Health) {
		auto HealthBarWidget = Cast<UHealthBarWidget>(HealthBar->GetUserWidgetObject());
		HealthBarWidget->ChangeHealthPercentage(Health);
		
		if (Health <= 0.5f && bBuilt)
		{
			for (auto system : FireParticleSystems)
			{
				if (!system->IsActive())
				{
					system->ActivateSystem();
                    FireSound->FadeIn(1.0f, 1.0f, 0.0f);
				}
			}
		}
		else
		{
			for (auto system : FireParticleSystems)
			{
                if (system->IsActive())
                {
					system->DeactivateSystem();
                    FireSound->FadeOut(1.0f, 0.0f);
                }
			}
		}

        if (Health <= 0)
        {
            OnBuildingDestroyed();
			Destroy();
        }
	});
}

void ABuilding::EndPlay(const EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);

    for (auto system : FireParticleSystems)
    {
        if (system)
        {
            system->DeactivateSystem();
        }
    }

    if (IsValid(City))
    {
        City->RemoveBuilding(this);
    }
}

void ABuilding::BeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
    if(Player && Player->GetOwner() == GetWorld()->GetFirstPlayerController())
        Tooltip->SetVisibility(true);
}

void ABuilding::EndMouseOver(UPrimitiveComponent* TouchedComponent)
{
    Tooltip->SetVisibility(false);
}

void ABuilding::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SetActorHiddenInGame(!bRevealed);
    //SetActorEnableCollision(bRevealed);
}

void ABuilding::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    HealthComponent->SetHealth(0.0f);
}

void ABuilding::ShowOutline()
{
    Mesh->SetRenderCustomDepth(true);
}

void ABuilding::HideOutline()
{
    Mesh->SetRenderCustomDepth(false);
}

bool ABuilding::IsConstructed() const
{
    return bBuilt;
}

bool ABuilding::IsDamaged()
{
	return HealthComponent->Health < HealthComponent->MaxHealth;
}

void ABuilding::ClientBuildingBuilt_Implementation()
{
    auto PC = Cast<AFrontierPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    if (PC->GetPlayerState<AFrontierPlayerState>()->Team == Player->Team)
    {
        auto Controller = Cast<AFrontierPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

        if (Controller)
        {
            Controller->GetBuildingConstructedEvent().ExecuteIfBound();
        }
    }
}

bool ABuilding::Construct(float ConstructionAmount)
{
    if (bBuilt)
        return true;

    if (HasAuthority())
    {
        auto GM = Cast<AFrontierGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        if (GM->bCheats)
            ConstructionAmount *= 9999.0f;
    }

	if (HealthComponent->AddHealth(ConstructionAmount))
	{
		bBuilt = true;
		Mesh->SetStaticMesh(BuildingMesh);

		Cast<AFrontierPlayerState>(Player)->PlayerStats.BuildingsBuilt++;
        OnBuildingConstructed();
        ClientBuildingBuilt();
	}

	return bBuilt;
}

bool ABuilding::Repair(float RepairAmount)
{
	return HealthComponent->AddHealth(RepairAmount);
}

void ABuilding::OnRep_Built()
{
    Mesh->SetStaticMesh(bBuilt ? BuildingMesh : ConstructionMesh);
}


void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuilding, Cost);
    DOREPLIFETIME(ABuilding, bBuilt);
    DOREPLIFETIME(ABuilding, City);
    DOREPLIFETIME(ABuilding, Player); 
}