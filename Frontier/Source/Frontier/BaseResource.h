// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resources.h"
#include "BaseResource.generated.h"

UCLASS()
class FRONTIER_API ABaseResource : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ABaseResource();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
    int32 ResourceAmount = 200;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontier Object")
    FString ResourceName = "BlankResource";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontier Object")
    FString ResourceDesc = "Resource description";

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EResource ResourceType;
    
    UFUNCTION(BlueprintCallable)
    int32 GetRemainingResources() const { return ResourceAmount; }

	UFUNCTION(BlueprintCallable)
	int Harvest(int GatherSpeed);

    UPROPERTY()
    bool bRevealed = false;
};
