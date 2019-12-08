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

		UpdateNav();

		switch (State)
		{
		case ECharacterStates::Idle:
			StateIdle();
			break;
		case ECharacterStates::Harvesting:
			StateHarvest();
			break;
		case ECharacterStates::Attacking:
			StateAttacking();
			break;
		case ECharacterStates::Building:
			StateBuilding();
			break;
		case ECharacterStates::Moving:
			StateMoving();
			break;
		default:
			break;
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
}

void AFrontierCharacter::EndPlay(EEndPlayReason::Type Reason)
{
    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (FrontierController)
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

		if (HasAuthority())
		{
			State = ECharacterStates::Moving;
			if (MoveObject->GetClass() == ALandscape::StaticClass())
				UAIBlueprintHelperLibrary::GetAIController(GetController())->MoveToLocation(Location, 60.f, false,true,false,true,0,true);
			else
				UAIBlueprintHelperLibrary::GetAIController(GetController())->MoveToActor(MoveObject, 70.f,false);
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
					else if (building->bCanDeposit)
						FinishDeposit();
				}
			}
			else
				State = ECharacterStates::Idle;
		}
		break;

	case ECharacterStates::Depositing:
		FinishDeposit();
		break;
	}
}

void AFrontierCharacter::UpdateNav()
{
	SetCanAffectNavigationGeneration(GetVelocity().Size() < 20.f);
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
		if (GetDistanceTo(MoveObject) > 200) //change to range
		{
			GetWorldTimerManager().ClearTimer(AttackTimerHandler);
			MoveTo(MoveObject);
		}
	}
	else
	{
		UE_LOG(LogFrontier, Display, TEXT("yo"));
		GetWorldTimerManager().ClearTimer(AttackTimerHandler);
		State = ECharacterStates::Idle; //change idle to find new target. on fail then go idle
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
		State = ECharacterStates::Harvesting;
	}
}

void AFrontierCharacter::FinishDeposit()
{
	Player->AddSpecificResources(HeldResources, HeldResourceType);
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

void AFrontierCharacter::SetAttack()
{
	if (IsValid(MoveObject))
	{
		State = ECharacterStates::Attacking;
		SetActorRelativeRotation(UFrontierHelperFunctionLibrary::LookAt(GetActorLocation(), MoveObject->GetActorLocation()));
		GetWorldTimerManager().SetTimer(AttackTimerHandler, this, &AFrontierCharacter::Attack, 1.0f, true); // check
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

void AFrontierCharacter::Attack()
{
	auto actorObject = Cast<AActor>(MoveObject);

	UHealthComponent* healthComponent = Cast<UHealthComponent>(actorObject->GetComponentByClass(UHealthComponent::StaticClass())); // check this

	if (IsValid(MoveObject))
	{
		healthComponent->ReceiveDamage((int)AttackStrength);
	}
}

void AFrontierCharacter::FindNewHarvest()
{
	TArray<ABaseResource*> Resources;

	for (TObjectIterator<ABaseResource> It; It; ++It)
	{
		if (It->GetClass() == HarvestObject->GetClass() && !(*It)->IsPendingKill()) //check this
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

void AFrontierCharacter::MoveTo(AActor* Actor)
{
	if (IsValid(GetController()))
	{
		MoveObject = Actor;
		UAIBlueprintHelperLibrary::GetAIController(GetController())->MoveToActor(MoveObject,70.f);
		State = ECharacterStates::Moving;
	}
}

void AFrontierCharacter::PauseTimerFunctions()
{
	GetWorldTimerManager().ClearTimer(ConstructTimerHandler);
	GetWorldTimerManager().ClearTimer(AttackTimerHandler);
	GetWorldTimerManager().ClearTimer(HarvestTimerHandler);
}

void AFrontierCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierCharacter, Player); 
	DOREPLIFETIME(AFrontierCharacter, State); 
	DOREPLIFETIME(AFrontierCharacter, MoveObject);
	DOREPLIFETIME(AFrontierCharacter, HarvestObject);
}