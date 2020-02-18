// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Classes/AIController.h"
#include "ExploreComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRONTIER_API UExploreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UExploreComponent();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditAnywhere)
    float SearchRadius = 2600.0f;

    UPROPERTY(EditAnywhere)
    float AcceptanceRadius = 200.0f;

protected:
	virtual void BeginPlay() override;

private:
    void PickTargetLocation();
    void OnMoveComplete(FAIRequestID RequestID, const FPathFollowingResult& Result);

    bool bExploring = true;

    UPROPERTY()
    AAIController* AIController;

    FDelegateHandle OnMoveCompleteDelegate;

    UPROPERTY(Replicated)
    FVector TargetLocation;
};
