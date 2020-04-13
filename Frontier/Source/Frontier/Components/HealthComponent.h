

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FHealthChangedEvent, AActor*, float);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTIER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	bool AddHealth(float Value);
	void SetHealth(float Value);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100;

	UFUNCTION(BlueprintCallable)
	bool ReceiveDamage(int Damage);

	UFUNCTION()
	void OnRep_Health();

	FHealthChangedEvent HealthChangeEvent;

protected:
	virtual void BeginPlay() override;
};
