// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "BaseResource.h"

// Sets default values
ABaseResource::ABaseResource()
{
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    if (RootComponent)
        RootComponent->SetMobility(EComponentMobility::Stationary);
}

// Called when the game starts or when spawned
void ABaseResource::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABaseResource::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

int ABaseResource::Harvest(int GatherSpeed)
{
	ResourceAmount -= GatherSpeed;
	if (ResourceAmount <= 0)
	{
		Destroy();
		return (GatherSpeed + ResourceAmount);
	}
	else
		return GatherSpeed;
}
