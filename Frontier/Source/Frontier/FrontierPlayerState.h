// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Resources.h"

#include "FrontierPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FRONTIER_API AFrontierPlayerState : public APlayerState
{
    GENERATED_BODY()

    UPROPERTY()
    FResources Resources;
};