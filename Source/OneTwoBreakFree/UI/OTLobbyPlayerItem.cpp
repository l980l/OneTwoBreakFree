// Fill out your copyright notice in the Description page of Project Settings.


#include "OTLobbyPlayerItem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UOTLobbyPlayerItem::SetPlayerInfo(const FString& InPlayerName, bool bInIsReady)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(InPlayerName));
    }

    if (ReadyStatusIcon)
    {
        ReadyStatusIcon->SetColorAndOpacity(bInIsReady ? FLinearColor::Green : FLinearColor::Red);
    }
}
