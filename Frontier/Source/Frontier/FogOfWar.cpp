// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FogOfWar.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "Frontier.h"

AFogOfWar::AFogOfWar() : WholeTexRegion(0, 0, 0, 0, TextureSize, TextureSize)
{
	PrimaryActorTick.bCanEverTick = false;

    Scale *= 2.0f;

    Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
    Plane->TranslucencySortPriority = 100;
    Plane->SetRelativeScale3D(FVector(Scale, Scale, 1.0f));
    Plane->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    
    RootComponent = Plane;
}

void AFogOfWar::BeginPlay()
{
    Super::BeginPlay();

    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();

    if (!GetOuter() || Player != FrontierController->PlayerState)
        SetActorHiddenInGame(true);
}

void AFogOfWar::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    Texture = UTexture2D::CreateTransient(TextureSize, TextureSize, PF_G8);
    Texture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
    Texture->SRGB = 0;
    Texture->UpdateResource();
    Texture->MipGenSettings = TMGS_NoMipmaps;

    Pixels = reinterpret_cast<uint8*>(FMemory::Malloc(TextureSize * TextureSize));

    for (int y = 0; y < TextureSize; ++y)
    {
        for (int x = 0; x < TextureSize; ++x)
        {
            Pixels[y * TextureSize + x] = 150;
        }
    }

    UpdateTextureRegions(0, 1, &WholeTexRegion, TextureSize, 1, Pixels, false);

    if (Material)
    {
        MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
        MaterialInstance->SetTextureParameterValue("FowTexture", Texture);
        Plane->SetMaterial(0, MaterialInstance);
    }
}

void AFogOfWar::RevealCircle(const FVector& Pos, float Radius)
{
    FVector2D Texel = FVector2D(Pos.X, Pos.Y) - FVector2D(GetActorLocation().X, GetActorLocation().Y);
    Texel = Texel * TextureSize / Scale;
    Texel += FVector2D(TextureSize / 2, TextureSize / 2);

    float TexelRadius = Radius * TextureSize / Scale;

    int MinX = FMath::Clamp<int>(Texel.X - TexelRadius, 0, TextureSize - 1);
    int MinY = FMath::Clamp<int>(Texel.Y - TexelRadius, 0, TextureSize - 1);
    int MaxX = FMath::Clamp<int>(Texel.X + TexelRadius, 0, TextureSize - 1);
    int MaxY = FMath::Clamp<int>(Texel.Y + TexelRadius, 0, TextureSize - 1);

    bool dirty = false;

    for (int x = MinX; x < MaxX; ++x)
    {
        for (int y = MinY; y < MaxY; ++y)
        {
            float distance = FVector2D::Distance(Texel, FVector2D(x, y));
            
            if (distance < TexelRadius)
            {
                static float smoothPct = 0.7f;

                uint8 oldVal = Pixels[y * TextureSize + x];
                float lerp = FMath::GetMappedRangeValueClamped(FVector2D(smoothPct, 1.0f), FVector2D(0, 1), distance / TexelRadius);
                uint8 newVal = lerp * 255;
                newVal = FMath::Min(newVal, oldVal);

                Pixels[y * TextureSize + x] = newVal;
                dirty = dirty || oldVal != newVal;
            }
        }
    }

    if (dirty)
        UpdateTextureRegions(0, 1, &WholeTexRegion, TextureSize, 1, Pixels, false);
}

void AFogOfWar::UpdateTextureRegions(int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
    if (!Texture || !Texture->Resource)
        return;

    struct FUpdateTextureRegionsData
    {
        FTexture2DResource* Texture2DResource;
        int32 MipIndex;
        uint32 NumRegions;
        FUpdateTextureRegion2D* Regions;
        uint32 SrcPitch;
        uint32 SrcBpp;
        uint8* SrcData;
    };

    FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

    RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
    RegionData->MipIndex = MipIndex;
    RegionData->NumRegions = NumRegions;
    RegionData->Regions = Regions;
    RegionData->SrcPitch = SrcPitch;
    RegionData->SrcBpp = SrcBpp;
    RegionData->SrcData = SrcData;

    ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
        UpdateTextureRegionsData, FUpdateTextureRegionsData*, RegionData, RegionData, bool, bFreeData, bFreeData,
        {
            for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
            {
                int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();

                if (RegionData->MipIndex >= CurrentFirstMip)
                {
                    RHIUpdateTexture2D(
                        RegionData->Texture2DResource->GetTexture2DRHI(),
                        RegionData->MipIndex - CurrentFirstMip,
                        RegionData->Regions[RegionIndex],
                        RegionData->SrcPitch,
                        RegionData->SrcData
                        + RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
                        + RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
                    );
                }
            }

            if (bFreeData)
            {
                FMemory::Free(RegionData->Regions);
                FMemory::Free(RegionData->SrcData);
            }

            delete RegionData;
        }
    );
}
