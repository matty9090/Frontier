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

UCLASS()
class UResearchNode : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Replicated)
    FString Name = "Unknown";

    UPROPERTY(BlueprintReadOnly, Replicated)
    EResearchType Type;

    UPROPERTY(BlueprintReadOnly, Replicated)
    FResources Cost;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TSubclassOf<AActor> Object;

    UPROPERTY(BlueprintReadOnly, Replicated)
    bool bUnlocked = false;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TArray<UResearchNode*> ChildNodes;

    UFUNCTION(BlueprintCallable)
    UResearchNode* AddChild(EResearchType InType, FResources InCost, TSubclassOf<AActor> InObject);

    bool IsSupportedForNetworking() const override { return true; }
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};