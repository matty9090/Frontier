#pragma once

#include "Resources.generated.h"

USTRUCT(BlueprintType)
struct FResources
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Wood;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Stone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Metal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Gold;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Food;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Population;
};