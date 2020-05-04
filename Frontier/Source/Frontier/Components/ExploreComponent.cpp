// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "ExploreComponent.h"
#include "FrontierCharacter.h"
#include "UnrealNetwork.h"
#include "Frontier.h"

UExploreComponent::UExploreComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


void UExploreComponent::BeginPlay()
{
	Super::BeginPlay();

    auto Character = Cast<AFrontierCharacter>(GetOwner());
    Character->State = ECharacterStates::Moving;

    AIController = Cast<AAIController>(Character->GetController());

    if (AIController)
    {
        auto PathComp = Cast<UPathFollowingComponent>(AIController->GetComponentByClass(UPathFollowingComponent::StaticClass()));
        OnMoveCompleteDelegate = PathComp->OnRequestFinished.AddUObject(this, &UExploreComponent::OnMoveComplete);

        if (GetOwner()->HasAuthority())
        {
            PickTargetLocation();
            AIController->MoveToLocation(TargetLocation, AcceptanceRadius);
        }
    }
}

void UExploreComponent::PickTargetLocation()
{
    FVector CurrentLocation = GetOwner()->GetActorLocation();
    auto RandomRotation = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, FMath::RandRange(0.0f, 360.0f)));

    TargetLocation = CurrentLocation + RandomRotation.RotateVector(FVector(1.0f, 0.0f, 0.0f)) * SearchRadius;
}

void UExploreComponent::OnMoveComplete(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (GetOwner()->HasAuthority())
    {
        PickTargetLocation();
        AIController->MoveToLocation(TargetLocation, -1.0f);
    }
}

void UExploreComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UExploreComponent, TargetLocation);
}
