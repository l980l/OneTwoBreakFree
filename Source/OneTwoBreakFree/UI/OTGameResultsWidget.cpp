// Fill out your copyright notice in the Description page of Project Settings.


#include "OTGameResultsWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "OneTwoBreakFree/PlayerState/OTPlayerState.h"
#include "OneTwoBreakFree/UI/OTResultPlayerItem.h"
#include "OneTwoBreakFree/GameMode/OTMatchGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "OneTwoBreakFree/GameState/OTMatchGameState.h"

void UOTGameResultsWidget::NativeConstruct()
{
	Super::NativeConstruct();

    SetupResultsScreen();
}

void UOTGameResultsWidget::SetupResultsScreen()
{
	if (!PlayerListBox)
		return;

    PlayerListBox->ClearChildren();

    if (AOTMatchGameState* GS = GetWorld()->GetGameState<AOTMatchGameState>())
    {
        RemainingTime = GS->ResultsScreenDuration;
    }

    UpdateTimerText();

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            TimerUpdateHandle,
            this,
            &UOTGameResultsWidget::UpdateTimerText,
            1.0f,
            true
        );
    }

    AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
    if (GameState)
    {
        for (APlayerState* PS : GameState->PlayerArray)
        {
            if (AOTPlayerState* OTPS = Cast<AOTPlayerState>(PS))
            {
                if (UOTResultPlayerItem* ResultPlayerItem = CreatePlayerItemWidget(OTPS))
                {
                    PlayerListBox->AddChild(ResultPlayerItem);
                    ResultPlayerItem->SetPadding(30.f);
                }
            }
        }
    }
}

void UOTGameResultsWidget::UpdateTimerText()
{
    if (TimerText)
    {
        int32 SecondsRemaining = FMath::CeilToInt(RemainingTime);
        SecondsRemaining = FMath::Clamp(SecondsRemaining, 0, SecondsRemaining);
        FString TimerString = FString::Printf(TEXT("You will be moved to the lobby in %d seconds."), SecondsRemaining);
        TimerText->SetText(FText::FromString(TimerString));
    }

    RemainingTime -= 1.0f;

    if (RemainingTime < 0.0f && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerUpdateHandle);
    }
}

UOTResultPlayerItem* UOTGameResultsWidget::CreatePlayerItemWidget(AOTPlayerState* PlayerState)
{
    if (!PlayerState || !PlayerItemWidgetClass || !PlayerListBox)
        return nullptr;

    UOTResultPlayerItem* PlayerItem = CreateWidget<UOTResultPlayerItem>(this, PlayerItemWidgetClass);
    if (!PlayerItem)
        return nullptr;

    PlayerItem->SetPlayerInfo(PlayerState);

    return PlayerItem;
}
