// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OneTwoBreakFree/Types/LobbyTypes.h"
#include "OTLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void PlayerReadyStateChanged(APlayerController* Player, bool bReady);

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void PlayerNameChanged(APlayerController* Player);

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    bool CanStartGame() const;

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    bool IsPlayerReady(APlayerController* Player) const;

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    TArray<FLobbyPlayerInfo> GetAllPlayersInfo() const;

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    int32 GetReadyPlayerCount() const;

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    int32 GetPlayerCount() const;

    void BroadcastLobbyState();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
    FName MatchLevelName = FName("Match");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
    int32 MinPlayersToStart = 2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
    int32 MaxPlayers = 4;

private:
    void SetGameInstancePlayerCount();
    bool FindPlayerInfo(APlayerController* Player, FLobbyPlayerInfo& OutInfo, int32& OutIndex) const;

    UPROPERTY()
    TArray<FLobbyPlayerInfo> LobbyPlayers;
};
