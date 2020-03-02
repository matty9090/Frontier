

#pragma once
#include "UnitTypeComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InfantryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTIER_API UInfantryComponent : public UUnitTypeComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInfantryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual int AffectDamage(int Damage, UUnitTypeComponent* TargetType) override;

		
};
