// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "BaseResource.h"
#include "Net/UnrealNetwork.h"
#include "Frontier.h"

ABaseResource::ABaseResource()
{
    PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);

	if (RootComponent)
	{
        RootComponent->SetMobility(EComponentMobility::Stationary);
	}
}

void ABaseResource::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseResource::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	SetActorHiddenInGame(!bRevealed);
}

int ABaseResource::Harvest(int GatherSpeed)
{
	ResourceAmount -= GatherSpeed;
	if (ResourceAmount <= 0)
	{
		Destroy(false, false);
		return (GatherSpeed + ResourceAmount);
	}
	else
		return GatherSpeed;
}

void ABaseResource::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseResource, ResourceAmount);
}