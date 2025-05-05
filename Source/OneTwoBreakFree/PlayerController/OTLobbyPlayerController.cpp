// Fill out your copyright notice in the Description page of Project Settings.


#include "OTLobbyPlayerController.h"
#include "OneTwoBreakFree/GameMode/OTLobbyGameMode.h"
#include "Net/UnrealNetwork.h"
#include "OneTwoBreakFree/UI/OTLobbyHUD.h"
#include "OneTwoBreakFree/GameInstance/OTGameInstance.h"
#include "OneTwoBreakFree/UI/OTLobbyUI.h"
#include "GameFramework/PlayerState.h"

void AOTLobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    OTLobbyHUD = Cast<AOTLobbyHUD>(GetHUD());

    if (OTLobbyHUD)
    {
        OTLobbyHUD->AddLobbyWidget();
    }

    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());
}

void AOTLobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOTLobbyPlayerController, bIsReadyForMatch);
}

void AOTLobbyPlayerController::ServerSetReadyState_Implementation(bool InbIsReadyForMatch)
{
    bIsReadyForMatch = InbIsReadyForMatch;

    AOTLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<AOTLobbyGameMode>();
    if (LobbyGameMode)
    {
        LobbyGameMode->PlayerReadyStateChanged(this, InbIsReadyForMatch);
    }
}

void AOTLobbyPlayerController::ServerStartGame_Implementation()
{
    if (!HasAuthority() || !IsLocalPlayerController())
        return;

    AOTLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<AOTLobbyGameMode>();
    if (LobbyGameMode && LobbyGameMode->CanStartGame())
    {
        LobbyGameMode->StartGame();
    }
}

void AOTLobbyPlayerController::ClientUpdateLobbyState_Implementation(const TArray<FLobbyPlayerInfo>& PlayersInfo, bool bCanStart)
{
    if (!OTLobbyHUD)
        OTLobbyHUD = Cast<AOTLobbyHUD>(GetHUD());
    if (OTLobbyHUD && OTLobbyHUD->LobbyWidget)
    {
        OTLobbyHUD->LobbyWidget->UpdateLobbyState(PlayersInfo, bCanStart);
        GetWorld()->GetTimerManager().ClearTimer(InitPlayerReadyStateTimerHandle);
    }

    else
    {
        GetWorld()->GetTimerManager().SetTimer(
            InitPlayerReadyStateTimerHandle,
            [this, PlayersInfo, bCanStart]()
            {
                if (!OTLobbyHUD)
                    OTLobbyHUD = Cast<AOTLobbyHUD>(GetHUD());
                if (OTLobbyHUD && OTLobbyHUD->LobbyWidget)
                {
                    OTLobbyHUD->LobbyWidget->UpdateLobbyState(PlayersInfo, bCanStart);
                }
            },
            0.5f,
            false
        );
    }
}

void AOTLobbyPlayerController::SetReadyState(bool InbIsReadyForMatch)
{
    ServerSetReadyState(InbIsReadyForMatch);
}

void AOTLobbyPlayerController::RequestStartGame()
{
    ServerStartGame();
}

void AOTLobbyPlayerController::ServerSetPlayerName_Implementation(const FString& NewName)
{
    if (NewName.IsEmpty() || NewName.Len() > 16)
        return;

    if (PlayerState)
    {
        PlayerState->SetPlayerName(NewName);

        UOTGameInstance* GI = Cast<UOTGameInstance>(GetGameInstance());
        if (GI)
        {
            GI->SavePlayerName(PlayerState->GetUniqueId(), NewName);
        }
    }

    AOTLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<AOTLobbyGameMode>();
    if (LobbyGameMode)
    {
        LobbyGameMode->PlayerNameChanged(this);
    }
}

void AOTLobbyPlayerController::SetPlayerNewName(const FString& NewName)
{
    ServerSetPlayerName(NewName);
}

void AOTLobbyPlayerController::ClientSetLoadingUIVisible_Implementation()
{
    SetLoadingUIVisibile();
}

void AOTLobbyPlayerController::SetLoadingUIVisibile()
{
    if (!OTLobbyHUD)
        OTLobbyHUD = Cast<AOTLobbyHUD>(GetHUD());
    if (OTLobbyHUD && OTLobbyHUD->LobbyWidget)
    {
        OTLobbyHUD->LobbyWidget->SetLoadingUIVisible();
    }
}
