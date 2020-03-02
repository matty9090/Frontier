#include "CavalryComponent.h"
#include "InfantryComponent.h"

// Sets default values for this component's properties
UCavalryComponent::UCavalryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCavalryComponent::BeginPlay()
{
	Super::BeginPlay();
}

int UCavalryComponent::AffectDamage(int Damage, UUnitTypeComponent* TargetType)
{
	if (TargetType->GetClass() == UInfantryComponent::StaticClass())
		return Damage * DamageMultiplier;

	return Damage;
}

