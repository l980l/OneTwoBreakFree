// Fill out your copyright notice in the Description page of Project Settings.


#include "OTLobbyUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"
#include "OneTwoBreakFree/PlayerController/OTLobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "OTLobbyPlayerItem.h"


void UOTLobbyUI::NativeConstruct()
{
    Super::NativeConstruct();

    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &UOTLobbyUI::OnReadyButtonClicked);
    }

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UOTLobbyUI::OnStartGameButtonClicked);
    }

    PlayerController = Cast<AOTLobbyPlayerController>(GetOwningPlayer());

    if (PlayerController)
    {
        bool bIsHost = PlayerController->IsLocalPlayerController() && PlayerController->HasAuthority();

        if (StartGameButton)
        {
            StartGameButton->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
            StartGameButton->SetIsEnabled(false);
        }

        if (ReadyButton)
        {
            ReadyButton->SetVisibility(bIsHost ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
            ReadyButton->SetIsEnabled(bIsHost ? false : true);
        }
    }

    if (ReadyButton && PlayerController)
    {
        UpdateReadyButtonText(PlayerController->bIsReadyForMatch);
    }

    if (GameStatusText)
    {
        GameStatusText->SetText(FText::FromString("Waiting for players to join..."));
    }

    if (SetNameButton)
    {
        SetNameButton->OnClicked.AddDynamic(this, &UOTLobbyUI::OnSetNameButtonClicked);
    }

    FTimerHandle InitPlayerNameTimerHandle;

    if (PlayerController)
    {
        GetWorld()->GetTimerManager().SetTimer(
            InitPlayerNameTimerHandle,
            this,
            &UOTLobbyUI::TryInitPlayerName,
            0.5f,
            false
        );
    }
}

void UOTLobbyUI::OnReadyButtonClicked()
{
    if (PlayerController)
    {
        bool bNewReadyState = !PlayerController->bIsReadyForMatch;
        PlayerController->SetReadyState(bNewReadyState);
        UpdateReadyButtonText(bNewReadyState);
    }
}

void UOTLobbyUI::OnStartGameButtonClicked()
{
    if (PlayerController)
    {
        PlayerController->RequestStartGame();
    }
}

void UOTLobbyUI::OnSetNameButtonClicked()
{
    if (!PlayerController || !PlayerNameTextBox)
        return;

    FString NewName = PlayerNameTextBox->GetText().ToString();

    if (NewName.IsEmpty())
        return;

    PlayerController->SetPlayerNewName(NewName);
}

void UOTLobbyUI::TryInitPlayerName()
{
    if (PlayerNameTextBox && PlayerController && PlayerController->PlayerState)
    {
        PlayerNameTextBox->SetText(FText::FromString(PlayerController->PlayerState->GetPlayerName()));
    }
    else
    {
        PlayerNameTextBox->SetText(FText::FromString("Player"));
    }
}

void UOTLobbyUI::UpdateLobbyState(const TArray<FLobbyPlayerInfo>& PlayersInfo, bool bCanStart)
{
    UpdatePlayerList(PlayersInfo);

    if (PlayersStatusText)
    {
        int32 ReadyCount = 0;
        for (const FLobbyPlayerInfo& Info : PlayersInfo)
        {
            if (Info.bIsReadyForMatch)
                ReadyCount++;
        }

        int32 TotalPlayers = PlayersInfo.Num();

        PlayersStatusText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d Players are Ready"), ReadyCount, TotalPlayers)));
    }

    if (GameStatusText)
    {
        if (bCanStart)
        {
            GameStatusText->SetText(FText::FromString("All Players are ready!"));
        }
        else if (PlayersInfo.Num() < 2)
        {
            GameStatusText->SetText(FText::FromString("Need more players to start the game"));
        }
        else
        {
            GameStatusText->SetText(FText::FromString("Waiting for all players to be ready"));
        }
    }

    // 시작 버튼 상태 업데이트 (호스트만 해당)
    if (StartGameButton && PlayerController)
    {
        bool bIsHost = PlayerController->IsLocalPlayerController() && PlayerController->HasAuthority();
        if (bIsHost)
        {
            StartGameButton->SetIsEnabled(bCanStart);

            // 애니메이션 재생 (시작 가능해졌을 때)
            static bool bWasAbleToStart = false;
            if (bCanStart && !bWasAbleToStart && ReadyPulseAnimation)
            {
                PlayAnimation(ReadyPulseAnimation);
            }
            bWasAbleToStart = bCanStart;
        }
    }
}

void UOTLobbyUI::UpdateReadyButtonText(bool bIsReady)
{
    if (ReadyButtonText)
    {
        FText ButtonText = bIsReady ? FText::FromString("Cancel") : FText::FromString("Ready");

        ReadyButtonText->SetText(ButtonText);
    }
}

void UOTLobbyUI::UpdatePlayerList(const TArray<FLobbyPlayerInfo>& PlayersInfo)
{
    if (!PlayerListBox)
        return;

    PlayerListBox->ClearChildren();

    bool bIsLocalHost = PlayerController && PlayerController->IsLocalPlayerController() && PlayerController->HasAuthority();

    for (const FLobbyPlayerInfo& PlayerInfo : PlayersInfo)
    {
        bool bIsHost = false;
        if (PlayerInfo.PlayerController)
        {
            bIsHost = bIsLocalHost && (PlayerInfo.PlayerController == PlayerController);
        }

        // 블루프린트 구현 이벤트를 통해 플레이어 항목 위젯 생성
        UUserWidget* PlayerItem = CreatePlayerListItem(
            PlayerInfo.PlayerName,
            PlayerInfo.bIsReadyForMatch,
            bIsHost
        );

        if (PlayerItem)
        {
            PlayerListBox->AddChild(PlayerItem);
        }
    }
}

void UOTLobbyUI::SetLoadingUIVisible()
{
    if (LoadingBoarder)
    {
        LoadingBoarder->SetVisibility(ESlateVisibility::Visible);
    }

    if (LoadingText)
    {
        LoadingText->SetVisibility(ESlateVisibility::Visible);
    }
}

UUserWidget* UOTLobbyUI::CreatePlayerListItem(const FString& PlayerName, bool bIsReady, bool bIsHost)
{
    if (!PlayerListItemClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerListItemClass is not set in LobbyUI!"));
        return nullptr;
    }

    if (GetOwningPlayer())
    {
        UOTLobbyPlayerItem* PlayerItem = CreateWidget<UOTLobbyPlayerItem>(GetOwningPlayer(), PlayerListItemClass);
        if (PlayerItem)
        {
            PlayerItem->SetPlayerInfo(PlayerName, bIsReady, bIsHost);
            return PlayerItem;
        }
    }

    return nullptr;
}
