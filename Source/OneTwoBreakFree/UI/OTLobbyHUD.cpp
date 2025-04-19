// Fill out your copyright notice in the Description page of Project Settings.


#include "OTLobbyHUD.h"
#include "OTLobbyUI.h"

void AOTLobbyHUD::AddLobbyWidget()
{
    APlayerController* PC = GetOwningPlayerController();
    if (LobbyUIClass && PC)
    {
        LobbyWidget = CreateWidget<UOTLobbyUI>(PC, LobbyUIClass);
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
        }
    }
}
