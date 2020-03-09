// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FogOfWar.h"
#include "EngineUtils.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/RevealFogComponent.h"
#include "FrontierPlayerState.h"
#include "FrontierPlayerController.h"
#include "City.h"
#include "FrontierCharacter.h"
#include "Buildings/Building.h"
#include "Frontier.h"

AFogOfWar::AFogOfWar() : WholeTexRegion(0, 0, 0, 0, TextureSize, TextureSize)
{
	PrimaryActorTick.bCanEverTick = true;

    Scale *= 2.0f;

    Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalFog"));
    Decal->SetDecalMaterial(Material);
    Decal->DecalSize = FVector(1000.0f, Scale, Scale);
    Decal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	Decal->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
    
    RootComponent = Decal;
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
    // Texture->MipGenSettings = TMGS_NoMipmaps; // Only available in Development mode?

    Pixels = reinterpret_cast<uint8*>(FMemory::Malloc(TextureSize * TextureSize));

    for (int Y = 0; Y < TextureSize; ++Y)
    {
        for (int X = 0; X < TextureSize; ++X)
        {
            Pixels[Y * TextureSize + X] = 255;
        }
    }

    UpdateTextureRegions(0, 1, &WholeTexRegion, TextureSize, 1, Pixels, false);

    if (Decal)
    {
        MaterialInstance = Decal->CreateDynamicMaterialInstance();
        MaterialInstance->SetTextureParameterValue("FowTexture", Texture);
    }
}

void AFogOfWar::Tick(float DeltaTime)
{
    UpdateActorsVisibility<ABuilding>(true);
    UpdateActorsVisibility<ABaseResource>(true);
    UpdateActorsVisibility<AFrontierCharacter>(false);

    for (int Y = 0; Y < TextureSize; ++Y)
    {
        for (int X = 0; X < TextureSize; ++X)
        {
            auto i = Y * TextureSize + X;
            
            if (Pixels[i] < KnownOpacity)
            {
                Pixels[i] = KnownOpacity;
            }
        }
    }

    auto FrontierController = GetWorld()->GetFirstPlayerController<AFrontierPlayerController>();
    
    for (TObjectIterator<URevealFogComponent> It; It; ++It)
    {
        auto Actor = It->GetOwner();
        auto Prop = Actor->GetClass()->FindPropertyByName("Player");

        if (Prop)
        {
            UObject* Obj = Cast<UObjectPropertyBase>(Prop)->GetObjectPropertyValue_InContainer(Actor);

            if (Obj && Cast<AFrontierPlayerState>(Obj) && FrontierController->PlayerState)
            {
                if (Cast<AFrontierPlayerState>(Obj)->Team == Cast<AFrontierPlayerState>(FrontierController->PlayerState)->Team)
                {
                    RevealCircle(Actor->GetActorLocation(), It->FogRadius);
                }
            }
        }
    }

    UpdateTextureRegions(0, 1, &WholeTexRegion, TextureSize, 1, Pixels, false);
}

void AFogOfWar::RevealCircle(const FVector& Pos, float Radius)
{
    auto Texel = WorldPositionToFog(Pos);
    float TexelRadius = Radius * TextureSize / (Scale * 0.75f);

    int MinX = FMath::Clamp<int>(Texel.X - TexelRadius, 0, TextureSize - 1);
    int MinY = FMath::Clamp<int>(Texel.Y - TexelRadius, 0, TextureSize - 1);
    int MaxX = FMath::Clamp<int>(Texel.X + TexelRadius, 0, TextureSize - 1);
    int MaxY = FMath::Clamp<int>(Texel.Y + TexelRadius, 0, TextureSize - 1);

    bool dirty = false;

    for (int X = MinX; X < MaxX; ++X)
    {
        for (int Y = MinY; Y < MaxY; ++Y)
        {
            float distance = FVector2D::Distance(Texel, FVector2D(X, Y));
            
            if (distance < TexelRadius)
            {
                static float smoothPct = 0.7f;

                uint8 OldVal = Pixels[Y * TextureSize + X];
                float Lerp = FMath::GetMappedRangeValueClamped(FVector2D(smoothPct, 1.0f), FVector2D(0, 1), distance / TexelRadius);
                uint8 NewVal = Lerp * 255;
                NewVal = FMath::Min(NewVal, OldVal);

                Pixels[Y * TextureSize + X] = NewVal;
                dirty = dirty || OldVal != NewVal;
            }
        }
    }

    if (dirty)
    {
        UpdateTextureRegions(0, 1, &WholeTexRegion, TextureSize, 1, Pixels, false);
        MaterialInstance->SetTextureParameterValue("FowTexture", Texture);
    }
}

bool AFogOfWar::IsRevealedBox(const FVector& Pos, float SizeX, float SizeY)
{
    auto Texel = WorldPositionToFog(Pos);
    float Scaled = static_cast<float>(TextureSize) / Scale;
    float TexelSizeX = SizeX * Scaled, TexelSizeY = SizeY * Scaled;

    int MinX = FMath::Clamp<int>(Texel.X - TexelSizeX, 0, TextureSize - 1);
    int MinY = FMath::Clamp<int>(Texel.Y - TexelSizeY, 0, TextureSize - 1);
    int MaxX = FMath::Clamp<int>(Texel.X + TexelSizeX, 0, TextureSize - 1);
    int MaxY = FMath::Clamp<int>(Texel.Y + TexelSizeY, 0, TextureSize - 1);

    bool bResult = true;

    for (int Y = MinY; Y < MaxY; ++Y)
    {
        for (int X = MinX; X < MaxX; ++X)
        {
            uint8 Pixel = Pixels[Y * TextureSize + X];

            if (Pixel > 10)
                return false;
        }
    }

    return true;
}

FVector2D AFogOfWar::WorldPositionToFog(const FVector& Pos)
{
    FVector2D Texel = FVector2D(Pos.X, Pos.Y) - FVector2D(GetActorLocation().X, GetActorLocation().Y);
    Texel = Texel * TextureSize / Scale;
    Texel += FVector2D(TextureSize / 2, TextureSize / 2);

    return Texel;
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
