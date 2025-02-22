// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "FrontierHelperFunctionLibrary.h"

FString UFrontierHelperFunctionLibrary::GetResourceName(EResource Res)
{
    FString Name;

    switch (Res)
    {
        case EResource::Wood: Name = "Wood"; break;
        case EResource::Stone: Name = "Stone"; break;
        case EResource::Population: Name = "Population"; break;
        case EResource::MaxPop: Name = "Max Population"; break;
    }

    return Name;
}
