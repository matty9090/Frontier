// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "FrontierGameState.h"
#include "FrontierPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Research.h"
#include "WidgetAnimation.h"
#include "Frontier.h"

AFrontierGameState::AFrontierGameState()
{

}

void AFrontierGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    if (HasAuthority())
    {
        auto Player = Cast<AFrontierPlayerState>(PlayerState);
        Player->Team = NumTeams++;

        FrontierPlayers.Add(Player);
    }
}

void AFrontierGameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);

    if (HasAuthority())
    {
        for (int32 i = 0; i < FrontierPlayers.Num(); i++)
        {
            if (FrontierPlayers[i] == PlayerState)
            {
                FrontierPlayers.RemoveAt(i, 1);
                return;
            }
        }
    }
}

void AFrontierGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierGameState, NumTeams);
    DOREPLIFETIME(AFrontierGameState, FrontierPlayers);
}