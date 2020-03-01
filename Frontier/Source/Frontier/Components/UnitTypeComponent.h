

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitTypeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTIER_API UUnitTypeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitTypeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float damageMultiplier = 2;

public:	
	virtual int AffectDamage(int damage, UUnitTypeComponent* TargetType);
		
};
