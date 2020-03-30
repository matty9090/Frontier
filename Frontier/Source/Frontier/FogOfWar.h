// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "RHI.h"
#include "Components/DecalComponent.h"
#include "FogOfWar.generated.h"

class AFrontierPlayerState;

UCLASS()
class FRONTIER_API AFogOfWar : public AActor
{
	GENERATED_BODY()
	
public:	
	AFogOfWar();
    ~AFogOfWar() { FMemory::Free(Pixels); }

    void Tick(float DeltaTime) override;
    bool IsRevealedBox(const FVector& Pos, float SizeX, float SizeY);
    bool IsServerActorRevealed(AActor* Actor, int32 ClientID) const { return ServerInfo[Actor][ClientID]; }

    UPROPERTY()
    AFrontierPlayerState* Player;

    UPROPERTY(EditAnywhere)
    int32 Scale = 10000;

protected:
    virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
    FVector2D WorldPositionToFog(const FVector& Pos);
    void RevealCircle(uint8* FogPixels, const FVector& Pos, float Radius, bool IsServer);
    void UpdateTextureRegions(int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

    UPROPERTY(EditAnywhere)
    UDecalComponent* Decal;

    UPROPERTY(EditAnywhere)
    int32 TextureSize = 1024;

    UPROPERTY(EditAnywhere)
    int32 KnownOpacity = 55;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* Material = nullptr;

    UPROPERTY()
    UTexture2D* Texture = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* MaterialInstance = nullptr;

    uint8* Pixels;
    FUpdateTextureRegion2D WholeTexRegion;

    TMap<int32, uint8*> ServerPixels;
    TMap<AActor*, TMap<int32, bool>> ServerInfo;

    template <class T>
    void UpdateActorsVisibility(uint8* FogPixels, bool bServer, bool bRemainVisible, int32 Client);
};

template <class T>
inline void AFogOfWar::UpdateActorsVisibility(uint8* FogPixels, bool bServer, bool bRemainVisible, int32 Client = 0)
{
    for (TActorIterator<T> It(GetWorld()); It; ++It)
    {
        auto Actor = *It;
        auto Texel = WorldPositionToFog(Actor->GetActorLocation());
        auto TexelRadius = 20.0f * TextureSize / (Scale * 0.75f);

        int MinX = FMath::Clamp<int>(Texel.X - TexelRadius, 0, TextureSize - 1);
        int MinY = FMath::Clamp<int>(Texel.Y - TexelRadius, 0, TextureSize - 1);
        int MaxX = FMath::Clamp<int>(Texel.X + TexelRadius, 0, TextureSize - 1);
        int MaxY = FMath::Clamp<int>(Texel.Y + TexelRadius, 0, TextureSize - 1);

        bool Visible = true;

        for (int X = MinX; X < MaxX && Visible; ++X)
        {
            for (int Y = MinY; Y < MaxY; ++Y)
            {
                if (FogPixels[Y * TextureSize + X] >= KnownOpacity)
                {
                    Visible = false;
                    break;
                }
            }
        }

        if (bServer)
        {
            bRemainVisible ? ServerInfo.FindOrAdd(Actor).FindOrAdd(Client) |= Visible : ServerInfo.FindOrAdd(Actor).FindOrAdd(Client) = Visible;
        }
        else
        {
            bRemainVisible ? Actor->bRevealed |= Visible : Actor->bRevealed = Visible;
        }
    }
}
