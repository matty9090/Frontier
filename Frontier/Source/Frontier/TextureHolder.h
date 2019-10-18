#pragma once

#include "Engine/Texture2D.h"
#include "TextureHolder.generated.h"

USTRUCT(BlueprintType)
struct FTextureHolder
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Texture = nullptr;
};