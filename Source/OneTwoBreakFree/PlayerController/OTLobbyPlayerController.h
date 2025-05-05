// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OneTwoBreakFree/Types/LobbyTypes.h"
#include "OTLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerSetReadyState(bool InbIsReadyForMatch);

    UFUNCTION(Server, Reliable)
    void ServerStartGame();

    UFUNCTION(Client, Reliable)
    void ClientUpdateLobbyState(const TArray<FLobbyPlayerInfo>& PlayersInfo, bool bCanStart);

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void SetReadyState(bool InbIsReadyForMatch);

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void RequestStartGame();

    UFUNCTION(Server, Reliable)
    void ServerSetPlayerName(const FString& NewName);

    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetPlayerNewName(const FString& NewName);

    UFUNCTION(Client, Reliable)
    void ClientSetLoadingUIVisible();

    UFUNCTION()
    void SetLoadingUIVisibile();

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby")
    uint8 bIsReadyForMatch : 1 = false;

private:
    UPROPERTY()
    TObjectPtr<class AOTLobbyHUD> OTLobbyHUD;

    FTimerHandle InitPlayerReadyStateTimerHandle;
};
