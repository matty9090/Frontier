#pragma once

#include "Object.h"
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
    UPROPERTY(BlueprintReadOnly)
    FString Name = "Unknown";

    UPROPERTY(BlueprintReadOnly)
    EResearchType Type;

    UPROPERTY(BlueprintReadOnly)
    FResources Cost;

    UPROPERTY(BlueprintReadOnly)
    TSubclassOf<AActor> Object;

    UPROPERTY(BlueprintReadOnly)
    bool bUnlocked = false;

    UPROPERTY(BlueprintReadOnly)
    TArray<UResearchNode*> Children;

    UFUNCTION(BlueprintCallable)
    UResearchNode* AddChild(EResearchType InType, FResources InCost, TSubclassOf<AActor> InObject)
    {
        auto Node = NewObject<UResearchNode>();
        Node->Type = InType;
        Node->Cost = InCost;
        Node->Object = InObject;

        Children.Add(Node);
        return Node;
    }
};

UCLASS()
class UResearchManager : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UResearchNode* Root;

private:

};