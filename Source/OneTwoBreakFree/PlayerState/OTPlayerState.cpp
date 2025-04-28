// Fill out your copyright notice in the Description page of Project Settings.


#include "OTPlayerState.h"
#include "Net/UnrealNetwork.h"

AOTPlayerState::AOTPlayerState()
{
    DeathTime = -0.1f;
    EscapeTime = -0.1f;
    KillCount = 0;
    CharacterRole = EOTCharacterRole::ECR_None;
}

void AOTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOTPlayerState, DeathTime);
    DOREPLIFETIME(AOTPlayerState, EscapeTime);
    DOREPLIFETIME(AOTPlayerState, KillCount);
    DOREPLIFETIME(AOTPlayerState, CharacterRole);
}
