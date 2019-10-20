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
    Population  UMETA(DisplayName="Population"),
    MaxPop      UMETA(DisplayName="Max Population")
};

USTRUCT(BlueprintType)
struct FResources
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EResource, float> Resources;

    TArray<EResource> Keys;
    TArray<float> Values;

    FResources()
    {
        Resources.Add(EResource::Wood, 0);
        Resources.Add(EResource::Stone, 0);
        Resources.Add(EResource::Metal, 0);
        Resources.Add(EResource::Gold, 0);
        Resources.Add(EResource::Food, 0);
        Resources.Add(EResource::Population, 0);
        Resources.Add(EResource::MaxPop, 0);
    }

    FResources(int32 Wood, int32 Stone, int32 Metal, int32 Gold, int32 Food, int32 Population, int32 MaxPop = 0)
    {
        Resources.Add(EResource::Wood, Wood);
        Resources.Add(EResource::Stone, Stone);
        Resources.Add(EResource::Metal, Metal);
        Resources.Add(EResource::Gold, Gold);
        Resources.Add(EResource::Food, Food);
        Resources.Add(EResource::Population, Population);
        Resources.Add(EResource::MaxPop, MaxPop);
    }

    void operator+=(const FResources& Res)
    {
        Resources[EResource::Wood] += Res.Resources[EResource::Wood];
        Resources[EResource::Stone] += Res.Resources[EResource::Stone];
        Resources[EResource::Metal] += Res.Resources[EResource::Metal];
        Resources[EResource::Gold] += Res.Resources[EResource::Gold];
        Resources[EResource::Food] += Res.Resources[EResource::Food];
        Resources[EResource::Population] -= Res.Resources[EResource::Population];
        Resources[EResource::MaxPop] += Res.Resources[EResource::MaxPop];
    }

    void operator-=(const FResources& Res)
    {
        Resources[EResource::Wood] -= Res.Resources[EResource::Wood];
        Resources[EResource::Stone] -= Res.Resources[EResource::Stone];
        Resources[EResource::Metal] -= Res.Resources[EResource::Metal];
        Resources[EResource::Gold] -= Res.Resources[EResource::Gold];
        Resources[EResource::Food] -= Res.Resources[EResource::Food];
        Resources[EResource::Population] += Res.Resources[EResource::Population];
        Resources[EResource::MaxPop] -= Res.Resources[EResource::MaxPop];
    }

    FResources operator*(const FResources& Res)
    {
        FResources NewRes;
        NewRes.Resources[EResource::Wood] = Resources[EResource::Wood] * Res.Resources[EResource::Wood];
        NewRes.Resources[EResource::Stone] = Resources[EResource::Stone] * Res.Resources[EResource::Stone];
        NewRes.Resources[EResource::Metal] = Resources[EResource::Metal] * Res.Resources[EResource::Metal];
        NewRes.Resources[EResource::Gold] = Resources[EResource::Gold] * Res.Resources[EResource::Gold];
        NewRes.Resources[EResource::Food] = Resources[EResource::Food] * Res.Resources[EResource::Food];
        NewRes.Resources[EResource::Population] = Resources[EResource::Population] * Res.Resources[EResource::Population];
        NewRes.Resources[EResource::MaxPop] = Resources[EResource::MaxPop] * Res.Resources[EResource::MaxPop];

        return NewRes;
    }

    bool operator>=(const FResources& Res) const
    {
        return Resources[EResource::Wood] >= Res.Resources[EResource::Wood] &&
               Resources[EResource::Stone] >= Res.Resources[EResource::Stone] &&
               Resources[EResource::Metal] >= Res.Resources[EResource::Metal] &&
               Resources[EResource::Gold] >= Res.Resources[EResource::Gold] &&
               Resources[EResource::Food] >= Res.Resources[EResource::Food] &&
               Resources[EResource::Population] + Res.Resources[EResource::Population] <= Resources[EResource::MaxPop];
    }

    TMap<EResource, bool> GetResourcesAvailable(const FResources& Target) const
    {
        TMap<EResource, bool> Available;

        Available.Add(EResource::Wood, Resources[EResource::Wood] >= Target.Resources[EResource::Wood]);
        Available.Add(EResource::Stone, Resources[EResource::Stone] >= Target.Resources[EResource::Stone]);
        Available.Add(EResource::Metal, Resources[EResource::Metal] >= Target.Resources[EResource::Metal]);
        Available.Add(EResource::Gold, Resources[EResource::Gold] >= Target.Resources[EResource::Gold]);
        Available.Add(EResource::Food, Resources[EResource::Food] >= Target.Resources[EResource::Food]);
        Available.Add(EResource::Population, Resources[EResource::Population] + Target.Resources[EResource::Population] <= Resources[EResource::MaxPop]);

        return Available;
    }

    bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
    {
        if (Ar.IsLoading())
        {
            // Move data to Map
            Ar << Keys;
            Ar << Values;

            for (auto It = Keys.CreateConstIterator(); It; ++It)
                Resources.Add(Keys[It.GetIndex()], Values[It.GetIndex()]);
        }
        else {
            // Move data to Arrays
            Resources.GenerateKeyArray(Keys);
            Resources.GenerateValueArray(Values);
            Ar << Keys;
            Ar << Values;
        }

        Keys.Empty();
        Values.Empty();

        bOutSuccess = true;
        return true;
    }
};

template<>
struct TStructOpsTypeTraits<FResources> : public TStructOpsTypeTraitsBase2<FResources>
{
    enum
    {
        WithNetSerializer = true
    };
};