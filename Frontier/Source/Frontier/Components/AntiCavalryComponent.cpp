#include "AntiCavalryComponent.h"
#include "CavalryComponent.h"

// Sets default values for this component's properties
UAntiCavalryComponent::UAntiCavalryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UAntiCavalryComponent::BeginPlay()
{
	Super::BeginPlay();
}

int UAntiCavalryComponent::AffectDamage(int Damage, UUnitTypeComponent* TargetType)
{
	if (TargetType->GetClass() == UCavalryComponent::StaticClass())
		return Damage * DamageMultiplier;

	return Damage;
}

