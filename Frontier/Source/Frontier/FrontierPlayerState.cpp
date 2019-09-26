// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontierPlayerState.h"
#include "UnrealNetwork.h"

void AFrontierPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerState, Resources);
    DOREPLIFETIME(AFrontierPlayerState, Team);
}