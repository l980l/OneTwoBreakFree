// Fill out your copyright notice in the Description page of Project Settings.


#include "OTLobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "OneTwoBreakFree/PlayerController/OTLobbyPlayerController.h"
#include "OneTwoBreakFree/GameInstance/OTGameInstance.h"

void AOTLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!NewPlayer)
        return;

    if (NewPlayer && NewPlayer->PlayerState)
    {
        UOTGameInstance* GI = Cast<UOTGameInstance>(GetGameInstance());
        if (GI)
        {
            FString SavedName;
            if (GI->GetSavedPlayerName(NewPlayer->PlayerState->GetUniqueId(), SavedName))
            {
                NewPlayer->PlayerState->SetPlayerName(SavedName);
            }
        }
    }

    FLobbyPlayerInfo NewPlayerInfo;
    NewPlayerInfo.PlayerController = NewPlayer;

    APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
    if (PlayerState)
    {
        NewPlayerInfo.PlayerName = PlayerState->GetPlayerName();
    }
    else
    {
        NewPlayerInfo.PlayerName = FString::Printf(TEXT("Player %d"), GetPlayerCount() + 1);
    }

    // 자동으로 첫 번째 들어온 플레이어(서버)는 준비 완료 상태로 설정
    if (LobbyPlayers.Num() == 0)
    {
        NewPlayerInfo.bIsReadyForMatch = true;
    }

    LobbyPlayers.Add(NewPlayerInfo);

    BroadcastLobbyState();

    UE_LOG(LogTemp, Log, TEXT("Player joined lobby: %s. Current count: %d/%d"), *NewPlayerInfo.PlayerName, GetPlayerCount(), MaxPlayers);
}

void AOTLobbyGameMode::Logout(AController* Exiting)
{
    APlayerController* PC = Cast<APlayerController>(Exiting);
    if (PC)
    {
        for (int32 i = 0; i < LobbyPlayers.Num(); i++)
        {
            if (LobbyPlayers[i].PlayerController == PC)
            {
                UE_LOG(LogTemp, Log, TEXT("Player left lobby: %s"), *LobbyPlayers[i].PlayerName);
                LobbyPlayers.RemoveAt(i);
                break;
            }
        }
    }

    Super::Logout(Exiting);
    
    BroadcastLobbyState();
}

void AOTLobbyGameMode::PlayerReadyStateChanged(APlayerController* Player, bool bReady)
{
    if (!Player)
        return;

    FLobbyPlayerInfo PlayerInfo;
    int32 PlayerIndex;
    if (FindPlayerInfo(Player, PlayerInfo, PlayerIndex))
    {
        LobbyPlayers[PlayerIndex].bIsReadyForMatch = bReady;

        BroadcastLobbyState();

        UE_LOG(LogTemp, Log, TEXT("Player %s is now %s"),
            *PlayerInfo.PlayerName, bReady ? TEXT("ready") : TEXT("not ready"));
    }
}

void AOTLobbyGameMode::PlayerNameChanged(APlayerController* Player)
{
    if (!Player)
        return;

    FLobbyPlayerInfo PlayerInfo;
    int32 PlayerIndex;
    if (FindPlayerInfo(Player, PlayerInfo, PlayerIndex))
    {
        APlayerState* PS = Player->GetPlayerState<APlayerState>();
        if (PS)
        {
            LobbyPlayers[PlayerIndex].PlayerName = PS->GetPlayerName();
        }

        BroadcastLobbyState();
    }
}

bool AOTLobbyGameMode::CanStartGame() const
{
    if (GetPlayerCount() < MinPlayersToStart)
        return false;

    for (const FLobbyPlayerInfo& Player : LobbyPlayers)
    {
        if (!Player.bIsReadyForMatch)
            return false;
    }

    return true;
}

void AOTLobbyGameMode::StartGame()
{
    if (!CanStartGame())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start game - conditions not met"));
        return;
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AOTLobbyPlayerController* PC = Cast<AOTLobbyPlayerController>(It->Get());
        if (PC)
        {
            PC->ClientSetLoadingUIVisible();
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Starting game with %d players"), GetPlayerCount());

    const FString URL = TEXT("/Game/Levels/") + MatchLevelName.ToString() + TEXT("?listen");

    SetGameInstancePlayerCount();

    bUseSeamlessTravel = false;
    GetWorld()->ServerTravel(URL);
}

bool AOTLobbyGameMode::IsPlayerReady(APlayerController* Player) const
{
    FLobbyPlayerInfo PlayerInfo;
    int32 PlayerIndex;
    if (FindPlayerInfo(Player, PlayerInfo, PlayerIndex))
    {
        return PlayerInfo.bIsReadyForMatch;
    }
    return false;
}

TArray<FLobbyPlayerInfo> AOTLobbyGameMode::GetAllPlayersInfo() const
{
    return LobbyPlayers;
}

int32 AOTLobbyGameMode::GetReadyPlayerCount() const
{
    int32 ReadyCount = 0;
    for (const FLobbyPlayerInfo& Player : LobbyPlayers)
    {
        if (Player.bIsReadyForMatch)
            ReadyCount++;
    }
    return ReadyCount;
}

int32 AOTLobbyGameMode::GetPlayerCount() const
{
    return LobbyPlayers.Num();
}

void AOTLobbyGameMode::SetGameInstancePlayerCount()
{
    UOTGameInstance* GameInstance = Cast<UOTGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->PlayerCount = LobbyPlayers.Num();
    }
}

bool AOTLobbyGameMode::FindPlayerInfo(APlayerController* Player, FLobbyPlayerInfo& OutInfo, int32& OutIndex) const
{
    for (int32 i = 0; i < LobbyPlayers.Num(); i++)
    {
        if (LobbyPlayers[i].PlayerController == Player)
        {
            OutInfo = LobbyPlayers[i];
            OutIndex = i;
            return true;
        }
    }
    return false;
}

void AOTLobbyGameMode::BroadcastLobbyState()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AOTLobbyPlayerController* PlayerController = Cast<AOTLobbyPlayerController>(*It);
        if (PlayerController)
        {
            PlayerController->ClientUpdateLobbyState(GetAllPlayersInfo(), CanStartGame());
        }
    }
}
