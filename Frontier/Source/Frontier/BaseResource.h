// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resources.h"
#include "BaseResource.generated.h"

UENUM(BlueprintType)
enum class EResources : uint8
{
	Wood		UMETA(DisplayName = "Wood"),
	Stone		UMETA(DisplayName = "Stone"),
	Metal		UMETA(DisplayName = "Metal"),
	Gold		UMETA(DisplayName = "Gold"),
	Food		UMETA(DisplayName = "Food"),
	Population	UMETA(DisplayName = "Population")
};

UCLASS()
class FRONTIER_API ABaseResource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseResource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ResourceAmount = 200;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ResourceName = "BlankResource";

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EResource ResourceType;
	
	UFUNCTION(BlueprintCallable)
	int32 GetRemainingResources() const { return ResourceAmount; }
};
