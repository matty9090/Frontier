#include "HealthComponent.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent() 
{
	bReplicates = true;
	Health = MaxHealth;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UHealthComponent::AddHealth(float Value)
{
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
	HealthChangeEvent.Broadcast(GetOwner(), Health / MaxHealth);
	return Health >= MaxHealth;
}

void UHealthComponent::SetHealth(float Value)
{
	Health = FMath::Clamp(Value, 0.0f, MaxHealth);
	HealthChangeEvent.Broadcast(GetOwner(), Health / MaxHealth);
}

void UHealthComponent::ReceiveDamage(int Damage)
{
	Health -= Damage;
	HealthChangeEvent.Broadcast(GetOwner(), Health / MaxHealth);
}

void UHealthComponent::OnRep_Health()
{
	HealthChangeEvent.Broadcast(GetOwner(), Health / MaxHealth);
}
