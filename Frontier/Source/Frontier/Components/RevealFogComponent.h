// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RevealFogComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTIER_API URevealFogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URevealFogComponent();

    UPROPERTY(EditAnywhere)
    float FogRadius = 900.0f;
};
