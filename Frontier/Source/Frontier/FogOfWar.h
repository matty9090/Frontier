// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "RHI.h"
#include "FogOfWar.generated.h"

class AFrontierPlayerState;

UCLASS()
class FRONTIER_API AFogOfWar : public AActor
{
	GENERATED_BODY()
	
public:	
	AFogOfWar();
    ~AFogOfWar() { FMemory::Free(Pixels); }

    UPROPERTY()
    AFrontierPlayerState* Player;

    void RevealCircle(const FVector& Pos, float Radius);

protected:
    virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
    void UpdateTextureRegions(int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Plane;

    UPROPERTY(EditAnywhere)
    int32 Scale = 10000;

    UPROPERTY(EditAnywhere)
    int32 TextureSize = 512;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* Material = nullptr;

    UPROPERTY()
    UTexture2D* Texture = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* MaterialInstance = nullptr;

    uint8* Pixels;
    FUpdateTextureRegion2D WholeTexRegion;
};
