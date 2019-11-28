// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/HealthBarWidget.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "Projectile.h"
#include "FrontierGameState.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "FogOfWar.h"
#include "Frontier.h"

AFrontierCharacter::AFrontierCharacter()
{
    // Set size for player capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Overlap);
    
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthBar		= CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));

	HealthBar->SetupAttachment(RootComponent);

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    SetActorEnableCollision(true);
    ActorHitDelegate.BindUFunction(this, "OnHit");
    OnActorHit.Add(ActorHitDelegate);
}

void AFrontierCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (GetOwner())
    {
        auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

        if (FrontierController && FrontierController->PlayerState && Player->Team == Cast<AFrontierPlayerState>(FrontierController->PlayerState)->Team)
        {
            auto GS = Cast<AFrontierGameState>(UGameplayStatics::GetGameState(GetWorld()));
            FrontierController->FogOfWar->RevealCircle(GetActorLocation(), GS->FowRevealRadius);
        }
    }
}

void AFrontierCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->HealthChangeEvent.AddLambda([&](AActor* Actor, float Health) {
		auto HealthBarWidget = Cast<UHealthBarWidget>(HealthBar->GetUserWidgetObject());
		HealthBarWidget->ChangeHealthPercentage(Health);

		if (Health <= 0)
			Destroy();
	});
}

void AFrontierCharacter::EndPlay(EEndPlayReason::Type Reason)
{
    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();
    FrontierController->PlayerKilledEvent.ExecuteIfBound(this);
}

void AFrontierCharacter::ShowOutline()
{
    GetMesh()->SetRenderCustomDepth(true);
}

void AFrontierCharacter::HideOutline()
{
    GetMesh()->SetRenderCustomDepth(false);
}

void AFrontierCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierCharacter, Player);
}