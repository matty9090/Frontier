#pragma once

#include "ObjectMacros.h"
#include "Resources.generated.h"

UENUM(BlueprintType)
enum class EResource : uint8
{
    Wood        UMETA(DisplayName="Wood"),
    Stone       UMETA(DisplayName="Stone"),
    Metal       UMETA(DisplayName="Metal"),
    Gold        UMETA(DisplayName="Gold"),
    Food        UMETA(DisplayName="Food"),
    Population  UMETA(DisplayName="Population")
};

USTRUCT(BlueprintType)
struct FResources
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EResource, int32> Resources;

    FResources()
    {
        Resources.Add(EResource::Wood, 0);
        Resources.Add(EResource::Stone, 0);
        Resources.Add(EResource::Metal, 0);
        Resources.Add(EResource::Gold, 0);
        Resources.Add(EResource::Food, 0);
        Resources.Add(EResource::Population, 0);
    }
};