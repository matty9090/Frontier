#include "InfantryComponent.h"
#include "AntiCavalryComponent.h"

// Sets default values for this component's properties
UInfantryComponent::UInfantryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UInfantryComponent::BeginPlay()
{
	Super::BeginPlay();
}

int UInfantryComponent::AffectDamage(int Damage, UUnitTypeComponent* TargetType)
{
	if (TargetType->GetClass() == UAntiCavalryComponent::StaticClass())
		return Damage * DamageMultiplier;

	return Damage;
}

