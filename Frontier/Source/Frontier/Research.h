#pragma once

#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "Resources.h"
#include "Research.generated.h"

UENUM(BlueprintType)
enum class EResearchType : uint8
{
    Unit        UMETA(DisplayName = "Unit"),
    Building    UMETA(DisplayName = "Building")
};

UENUM(BlueprintType)
enum class EResearchState : uint8
{
    Researched  UMETA(DisplayName = "Researched"),
    Available   UMETA(DisplayName = "Available"),
    Locked      UMETA(DisplayName = "Locked")
};

UCLASS()
class UResearchNode : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Replicated)
    FString Name = "Unknown";

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_State)
    EResearchState State = EResearchState::Locked;

    UPROPERTY(BlueprintReadOnly, Replicated)
    EResearchType Type;

    UPROPERTY(BlueprintReadOnly, Replicated)
    FResources Cost;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TSubclassOf<AActor> Object = nullptr;

    UPROPERTY(BlueprintReadOnly, Replicated)
    UResearchNode* Parent = nullptr;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TArray<UResearchNode*> ChildNodes;

    UFUNCTION(BlueprintCallable)
    UResearchNode* AddChild(EResearchType InType, FResources InCost, TSubclassOf<AActor> InObject);

    UFUNCTION()
    void OnRep_State();

    void Refresh();
    void Traverse(TArray<UResearchNode*>& OutNodes);
    bool IsSupportedForNetworking() const override { return true; }
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};