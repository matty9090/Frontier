#include "UnitTypeComponent.h"

// Sets default values for this component's properties
UUnitTypeComponent::UUnitTypeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UUnitTypeComponent::BeginPlay()
{
	Super::BeginPlay();
}

int UUnitTypeComponent::AffectDamage(int Damage, UUnitTypeComponent* TargetType)
{
	return Damage;
}


