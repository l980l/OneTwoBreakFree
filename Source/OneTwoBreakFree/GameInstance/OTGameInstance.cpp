// Fill out your copyright notice in the Description page of Project Settings.


#include "OTGameInstance.h"

void UOTGameInstance::Init()
{
    Super::Init();

    PlayerCount = 0;
}

void UOTGameInstance::SavePlayerName(const FUniqueNetIdRepl& PlayerId, const FString& PlayerName)
{
    if (PlayerId.IsValid())
    {
        PlayerNameMap.Add(PlayerId->ToString(), PlayerName);
    }
}

bool UOTGameInstance::GetSavedPlayerName(const FUniqueNetIdRepl& PlayerId, FString& OutPlayerName)
{
    if (PlayerId.IsValid())
    {
        FString* FoundName = PlayerNameMap.Find(PlayerId->ToString());
        if (FoundName)
        {
            OutPlayerName = *FoundName;
            return true;
        }
    }
    return false;
}
