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

int UInfantryComponent::AffectDamage(int damage, UUnitTypeComponent* TargetType)
{
	if (TargetType->GetClass() == UAntiCavalryComponent::StaticClass())
		return damage * damageMultiplier;

	return damage;
}

