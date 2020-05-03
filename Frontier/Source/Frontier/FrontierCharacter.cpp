// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/RevealFogComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/HealthBarWidget.h"
#include "Widgets/FeedbackWidget.h"
#include "FrontierHelperFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Materials/Material.h"
#include "Buildings/Building.h"
#include "Buildings/TownHall.h"
#include "Engine/World.h"
#include "Landscape.h"
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
    
	HealthComponent    = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthBar		   = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    RevealFogComponent = CreateDefaultSubobject<URevealFogComponent>(TEXT("RevealFog"));

	HealthBar->SetupAttachment(RootComponent);

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    SetActorEnableCollision(true);
    ActorHitDelegate.BindUFunction(this, "OnHit");
    OnActorHit.Add(ActorHitDelegate);

	// Set up resource deposit widget
	ResourceDepositWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlusResourceWidget"));
	ResourceDepositWidget->SetWidgetSpace(EWidgetSpace::Screen);
	ResourceDepositWidget->SetDrawAtDesiredSize(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> ResWidget(TEXT("/Game/Frontier/Blueprints/Widgets/WBP_GenericFeedback"));

	if (ResWidget.Succeeded())
	{
		ResourceDepositWidget->SetWidgetClass(ResWidget.Class);
	}
}

void AFrontierCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	SetActorHiddenInGame(!bRevealed);

    if (HasAuthority())
    {
		UpdateNav();

		switch (State)
		{
		case ECharacterStates::Idle:
			StateIdle();
			SetCanAffectNavigationGeneration(true);
			break;
		case ECharacterStates::Harvesting:
			StateHarvest();
			SetCanAffectNavigationGeneration(true);
			break;
		case ECharacterStates::Attacking:
			StateAttacking();
			SetCanAffectNavigationGeneration(true);
			break;
		case ECharacterStates::Building:
			StateBuilding();
			SetCanAffectNavigationGeneration(true);
			break;
		case ECharacterStates::Moving:
			StateMoving();
			SetCanAffectNavigationGeneration(false);
			break;
		default:
			SetCanAffectNavigationGeneration(false);
			break;
		}
    }
}

void AFrontierCharacter::BeginPlay()
{
	Super::BeginPlay();

	UnitTypeComponent = Cast<UUnitTypeComponent>(GetComponentByClass(UUnitTypeComponent::StaticClass()));

	HealthComponent->HealthChangeEvent.AddLambda([&](AActor* Actor, float Health) {
		auto HealthBarWidget = Cast<UHealthBarWidget>(HealthBar->GetUserWidgetObject());
		HealthBarWidget->ChangeHealthPercentage(Health);

		if (Health <= 0)
			Destroy();
	});

	if (HasAuthority())
	{
		MoveCompleteDelegate.BindUFunction(this, "OnMoveCompleted");
		auto cont = GetController();

		if (cont != NULL)
		{
			auto ai = UAIBlueprintHelperLibrary::GetAIController(cont);
			ai->ReceiveMoveCompleted.Add(MoveCompleteDelegate);
		}
	}

	auto AI = UAIBlueprintHelperLibrary::GetAIController(this);

	if (AI)
	{
		auto Path = Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent());

		if (Path)
		{
			Path->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
			Path->SetCrowdAnticipateTurns(true);
			Path->SetCrowdSlowdownAtGoal(false);
			Path->SetCrowdOptimizeTopology(true);
		}
	}
}

void AFrontierCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (IsValid(FrontierController))
    {
        FrontierController->PlayerKilledEvent.ExecuteIfBound(this);
    }
}

void AFrontierCharacter::ShowOutline()
{
    GetMesh()->SetRenderCustomDepth(true);
}

void AFrontierCharacter::HideOutline()
{
    GetMesh()->SetRenderCustomDepth(false);
}

void AFrontierCharacter::MoveToLocation(FVector Location, AActor* Object)
{
	if (IsValid(Object))
	{
		MoveObject = Object;
		PauseTimerFunctions();

		if (MoveObject->GetClass()->IsChildOf(ABaseResource::StaticClass()))
			bHarvestMove = true;
		else
			bHarvestMove = false;

		if (HasAuthority())
		{
			State = ECharacterStates::Moving;

			if (MoveObject->GetClass() == ALandscape::StaticClass())
				UAIBlueprintHelperLibrary::GetAIController(GetController())->MoveToLocation(Location, -1.0f);
			else
				UAIBlueprintHelperLibrary::GetAIController(GetController())->MoveToActor(MoveObject, MoveRange, false);
		}
	}
}

void AFrontierCharacter::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result != EPathFollowingResult::Success)
		return;

	switch (State)
	{
	case ECharacterStates::Moving:
		if (IsValid(MoveObject))
		{
			if (MoveObject->GetClass()->IsChildOf(AFrontierCharacter::StaticClass()))
			{
				if (Player != UFrontierHelperFunctionLibrary::TryGetFrontierObjectPlayer(MoveObject))
					SetAttack();
				else
					State = ECharacterStates::Idle;

			}
			else if (MoveObject->GetClass()->IsChildOf(ABaseResource::StaticClass()))
			{
				SetHarvest();
			}
			else if (MoveObject->GetClass()->IsChildOf(ABuilding::StaticClass()))
			{
				if (Player != UFrontierHelperFunctionLibrary::TryGetFrontierObjectPlayer(MoveObject))
					SetAttack();
				else
				{
					auto building = Cast<ABuilding>(MoveObject);
					if (!building->bBuilt)
						SetBuild();
					else if (building->IsDamaged())
						SetRepair();
					else if (building->bCanDeposit)
						FinishDeposit();
				}
			}
			else
				State = ECharacterStates::Idle;
		}
		else
		{
			if(bHarvestMove)
				FindNewHarvest();
		}

		break;

	case ECharacterStates::Depositing:
		FinishDeposit();
		break;
	}
}

void AFrontierCharacter::UpdateNav()
{
	// SetCanAffectNavigationGeneration(GetVelocity().Size() < 20.f);
}

void AFrontierCharacter::StateIdle()
{

}

void AFrontierCharacter::StateHarvest()
{
	if (HeldResources < HeldResourceLimit)
	{
		if (IsValid(HarvestObject))
		{
			if (!GetWorldTimerManager().IsTimerActive(HarvestTimerHandler))
			{
				SetActorRelativeRotation(UFrontierHelperFunctionLibrary::LookAt(GetActorLocation(), HarvestObject->GetActorLocation()));
				GetWorldTimerManager().SetTimer(HarvestTimerHandler, this, &AFrontierCharacter::HarvestResource, 1.0f, true);
			}
		}
		else
		{
			FindNewHarvest();
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(HarvestTimerHandler);
		TArray<ABuilding*> Buildings;

		for (TObjectIterator<ABuilding> It; It; ++It)
		{
			if (IsValid(*It) && (*It)->bCanDeposit)
			{
				Buildings.Add(*It);
			}
		}

		MoveTo(UFrontierHelperFunctionLibrary::GetClosestBuilding(GetActorLocation(), Player, Buildings));
		State = ECharacterStates::Depositing;
	}
}

void AFrontierCharacter::StateAttacking()
{
	if (IsValid(MoveObject))
	{
		SetActorRelativeRotation(UFrontierHelperFunctionLibrary::LookAt(GetActorLocation(), MoveObject->GetActorLocation()));
		if (GetDistanceTo(MoveObject) > AttackRange && !MoveObject->GetClass()->IsChildOf(ABuilding::StaticClass()))
		{
			GetWorldTimerManager().ClearTimer(AttackTimerHandler);
			MoveTo(MoveObject);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(AttackTimerHandler);
		FindNewEnemy();
	}
}

void AFrontierCharacter::StateBuilding()
{

}

void AFrontierCharacter::StateMoving()
{

}

void AFrontierCharacter::SetHarvest()
{
	if (bCanGather)
	{
		auto resource = Cast<ABaseResource>(MoveObject);
		HarvestObject = resource;
		LastHarvestClass = HarvestObject->GetClass();
		State = ECharacterStates::Harvesting;
	}
}

void AFrontierCharacter::ClientFinishDeposit_Implementation(float Res, EResource Type)
{
	auto PC = Cast<AFrontierPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC->GetPlayerState<AFrontierPlayerState>()->Team == Player->Team)
	{
		auto ResWidget = Cast<UFeedbackWidget>(ResourceDepositWidget->GetUserWidgetObject());
		ResWidget->Text = "<Green>+" + FString::FromInt(Res) + "</> ";
		ResWidget->Text += UFrontierHelperFunctionLibrary::GetResourceName(Type);
		ResWidget->Play();

		ResourceDepositWidget->SetWorldLocation(MoveObject->GetActorLocation() + FVector::UpVector * 60.0f);
	}
}

void AFrontierCharacter::FinishDeposit()
{
	float AddedRes = Player->AddSpecificResources(HeldResources, HeldResourceType);

	if (HeldResources > 0.0f)
	{
		ClientFinishDeposit(AddedRes, HeldResourceType);
	}

	HeldResources = 0.f;

	if (IsValid(HarvestObject))
		MoveTo(HarvestObject);
	else
		FindNewHarvest();
}

void AFrontierCharacter::HarvestResource()
{
	if (IsValid(HarvestObject))
	{
		if (HarvestObject->ResourceType != HeldResourceType)
		{
			HeldResources = 0;
			HeldResourceType = HarvestObject->ResourceType;
		}

		HeldResources += HarvestObject->Harvest(GatherSpeed);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(HarvestTimerHandler);
		FindNewHarvest();
	}
}

void AFrontierCharacter::SetBuild()
{
	if (IsValid(MoveObject))
	{
		auto building = Cast<ABuilding>(MoveObject);

		if (!building->IsConstructed() && bCanGather)
		{
			State = ECharacterStates::Building;
			SetActorRelativeRotation(UFrontierHelperFunctionLibrary::LookAt(GetActorLocation(), MoveObject->GetActorLocation()));
			GetWorldTimerManager().SetTimer(ConstructTimerHandler, this, &AFrontierCharacter::Construct, 1.0f, true); 
		}
	}
}

void AFrontierCharacter::Construct()
{
	if (IsValid(MoveObject))
	{
		auto building = Cast<ABuilding>(MoveObject);

		if (building->Construct(BuildSpeed))
		{
			GetWorldTimerManager().ClearTimer(ConstructTimerHandler);
			State = ECharacterStates::Idle;
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ConstructTimerHandler);
	}
}


void AFrontierCharacter::SetRepair()
{
	if (IsValid(MoveObject))
	{
		auto building = Cast<ABuilding>(MoveObject);

		if (building->IsConstructed() && bCanGather)
		{
			State = ECharacterStates::Building;
			SetActorRelativeRotation(UFrontierHelperFunctionLibrary::LookAt(GetActorLocation(), MoveObject->GetActorLocation()));
			GetWorldTimerManager().SetTimer(RepairTimerHandler, this, &AFrontierCharacter::Repair, 1.0f, true);
		}
	}
}

void AFrontierCharacter::Repair()
{
	if (IsValid(MoveObject))
	{
		auto building = Cast<ABuilding>(MoveObject);
		
		if (building->Repair(BuildSpeed))
		{
			GetWorldTimerManager().ClearTimer(RepairTimerHandler);
			State = ECharacterStates::Idle;
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RepairTimerHandler);
	}
}

void AFrontierCharacter::SetAttack()
{
	if (IsValid(MoveObject))
	{
		State = ECharacterStates::Attacking;
		SetActorRelativeRotation(UFrontierHelperFunctionLibrary::LookAt(GetActorLocation(), MoveObject->GetActorLocation()));
		GetWorldTimerManager().SetTimer(AttackTimerHandler, this, &AFrontierCharacter::Attack, AttackTime, true); // check
	}
}

void AFrontierCharacter::Attack()
{
	if (IsValid(MoveObject))
	{
		if (bIsArcher)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector FirePoint = LaunchPosition->GetComponentLocation();
			auto Rotator = LaunchPosition->GetComponentRotation();

			auto Projectile = GetWorld()->SpawnActor<AProjectile>(
				ProjectileClass,
				FirePoint,
				Rotator,
				SpawnParams
			);

            Projectile->MovementComponent->HomingTargetComponent = MoveObject->GetRootComponent();
			Projectile->Player = Player;
		}
		else
		{
			auto actorObject = Cast<AActor>(MoveObject);
			UHealthComponent* healthComponent = Cast<UHealthComponent>(actorObject->GetComponentByClass(UHealthComponent::StaticClass())); 

			UUnitTypeComponent* typeComponent = Cast<UUnitTypeComponent>(actorObject->GetComponentByClass(UUnitTypeComponent::StaticClass()));

			auto damage = AttackStrength;

			if(typeComponent)
				damage = UnitTypeComponent->AffectDamage(AttackStrength, typeComponent);

			if (healthComponent->ReceiveDamage((int)damage))
				Player->PlayerStats.UnitsKilled++;
		}
	}
}

void AFrontierCharacter::FindNewHarvest()
{
	if (IsValid(LastHarvestClass))
	{
		TArray<ABaseResource*> Resources;

		for (TObjectIterator<ABaseResource> It; It; ++It)
		{
			if (It->GetClass() == LastHarvestClass && !(*It)->IsPendingKill()) //check this
			{
				Resources.Add(*It);
			}
		}

		if (Resources.Num() > 0)
		{
			auto closestResource = UFrontierHelperFunctionLibrary::GetClosestResource(GetActorLocation(), Player, Resources);
			auto building = Cast<ABaseResource>(closestResource);
			HarvestObject = closestResource;
			MoveTo(HarvestObject);
		}
	}
	else
		State = ECharacterStates::Idle;
}

void AFrontierCharacter::FindNewEnemy()
{
	if (HasAuthority())
	{
		auto Fog = Cast<AFogOfWar>(UGameplayStatics::GetActorOfClass(GetWorld(), AFogOfWar::StaticClass()));
		
		int32 Client;
		GetWorld()->GetGameState()->PlayerArray.Find(Player, Client);

		if (!IsValid(Fog))
			return;

		TArray<AFrontierCharacter*> Characters;

        for (TActorIterator<AFrontierCharacter> It(GetWorld(), AFrontierCharacter::StaticClass()); It; ++It)
		{
            if (IsValid(*It) && IsValid(It->Player) && *It != this && Fog->IsServerActorRevealed(*It, Client))
            {
                Characters.Add(*It);
            }
		}

		auto ClosestCharacter = UFrontierHelperFunctionLibrary::GetClosestEnemy(GetActorLocation(), Player, Characters);

		if (ClosestCharacter)
			MoveTo(ClosestCharacter);
		else
			State = ECharacterStates::Idle;
	}
}

void AFrontierCharacter::MoveTo(AActor* Actor)
{
	if (IsValid(GetController()))
	{
		MoveObject = Actor;
		State = ECharacterStates::Moving;
		UAIBlueprintHelperLibrary::GetAIController(GetController())->MoveToActor(MoveObject, MoveRange);
	}
}

void AFrontierCharacter::PauseTimerFunctions()
{
	GetWorldTimerManager().ClearTimer(ConstructTimerHandler);
	GetWorldTimerManager().ClearTimer(AttackTimerHandler);
	GetWorldTimerManager().ClearTimer(HarvestTimerHandler);
	GetWorldTimerManager().ClearTimer(RepairTimerHandler);
}

void AFrontierCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AFrontierCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierCharacter, Player); 
	DOREPLIFETIME(AFrontierCharacter, State); 
	DOREPLIFETIME(AFrontierCharacter, MoveObject);
	DOREPLIFETIME(AFrontierCharacter, HarvestObject);
	DOREPLIFETIME(AFrontierCharacter, LastHarvestClass);
}