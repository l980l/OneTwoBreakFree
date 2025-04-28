// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OneTwoBreakFree/Types/LobbyTypes.h"
#include "OTLobbyUI.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;
class UEditableTextBox;
class AOTLobbyPlayerController;
class UBorder;
class UOTLobbyPlayerItem;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTLobbyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StartGameButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayersStatusText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerListBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameStatusText;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ReadyPulseAnimation;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> PlayerNameTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SetNameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> LoadingBoarder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LoadingText;

public:
	virtual void NativeConstruct() override;

	void UpdateLobbyState(const TArray<struct FLobbyPlayerInfo>& PlayersInfo, bool bCanStart);
	void UpdateReadyButtonText(bool bIsReady);
	void UpdatePlayerList(const TArray<struct FLobbyPlayerInfo>& PlayersInfo);
	void SetLoadingUIVisible();

	UFUNCTION(Category = "Lobby")
	UUserWidget* CreatePlayerListItem(const FString& PlayerName, bool bIsReady, bool bIsHost);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UOTLobbyPlayerItem> PlayerListItemClass;

private:
	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnStartGameButtonClicked();

	UFUNCTION()
	void OnSetNameButtonClicked();

	void TryInitPlayerName();

    UPROPERTY()
    TObjectPtr<AOTLobbyPlayerController> PlayerController;
};
