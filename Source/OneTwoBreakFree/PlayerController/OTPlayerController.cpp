// Fill out your copyright notice in the Description page of Project Settings.


#include "OTPlayerController.h"
#include "OneTwoBreakFree/UI/OTCharacterOverlayWidget.h"
#include "OneTwoBreakFree/UI/OTAnnouncementWidget.h"
#include "OneTwoBreakFree/UI/OTHUD.h"
#include "OneTwoBreakFree/UI/OTLoadingUI.h"
#include "OneTwoBreakFree/UI/OTSpectatorUI.h"
#include "OneTwoBreakFree/UI/OTGameResultsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "OneTwoBreakFree/GameState/OTMatchGameState.h"
#include "OneTwoBreakFree/GameMode/OTMatchGameMode.h"
#include "Net/UnrealNetwork.h"

void AOTPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
        LoadingUI = CreateWidget<UOTLoadingUI>(this, LoadingUIClass);
        if (LoadingUI)
        {
            LoadingUI->AddToViewport();
        }
    }

    OTHUD = Cast<AOTHUD>(GetHUD());

    SetInputMode(FInputModeGameOnly());
}

void AOTPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MatchState == MatchState::InProgress && MatchStartTimestamp > 0.f)
    {
        float ElapsedTime = GetWorld()->GetTimeSeconds() - MatchStartTimestamp;
        SetHUDMatchTimeFromSeconds(ElapsedTime);
    }
}

void AOTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOTPlayerController, MatchState);
}

void AOTPlayerController::SetHUDHealth(float HealthPercent)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetHealthPercent(HealthPercent);
    }
}

void AOTPlayerController::SetHUDHealthMarquee(bool bMarquee)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetHealthMarquee(bMarquee);
    }
}

void AOTPlayerController::SetHUDStamina(float StaminaPercent)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetStaminaPercent(StaminaPercent);
    }
}

void AOTPlayerController::ShowHUDStamina(bool bShow)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->ShowStaminaWidget(bShow);
    }
}

void AOTPlayerController::SetHUDMatchTime(int32 Minutes, int32 Seconds)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetMatchTimeText(Minutes, Seconds);
    }
}

void AOTPlayerController::SetHUDMatchTimeFromSeconds(float TotalSeconds)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetMatchTimeFromSeconds(TotalSeconds);
    }
}

void AOTPlayerController::SetupKillerWidget()
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetupKillerWidget();
    }
}

void AOTPlayerController::SetHUDRifleAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetRifleAmmo(CurrentAmmo, MaxAmmo);
    }
}

void AOTPlayerController::SetHUDBazookaPercent(float BazookaPercent)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetBazookaPercent(BazookaPercent);
    }
}

void AOTPlayerController::FlashCrosshairRed()
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->FlashCrosshairRed();
    }
}

void AOTPlayerController::FlashHitMarker()
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->FlashHitMarker();
    }
}

void AOTPlayerController::HideLoadingUI()
{
    if (LoadingUI)
    {
        LoadingUI->HideLoadingUI();
    }
}

void AOTPlayerController::ShowAnnouncement(EAnnouncementType Type, float ShowTime)
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD && OTHUD->Announcement)
    {
        OTHUD->Announcement->ShowAnnouncement(Type, ShowTime);
    }
}

void AOTPlayerController::ToggleSettingUI()
{
    if (!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());
    if (OTHUD)
    {
        OTHUD->ToggleSettingUI();
    }
}

void AOTPlayerController::ClientSetSpectatorUI_Implementation()
{
    if (OTHUD)
    {
        OTHUD->AddSpectatorWidget();
    }

    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AOTPlayerController::EnterSpectatorMode()
{
    bIsSpectating = true;

    ClientSetSpectatorUI();
}

void AOTPlayerController::SetSpectatingPlayerInfo(const FString& PlayerName)
{
    SpectatingPlayerName = PlayerName;

    if (IsLocalController())
    {
        if (OTHUD && OTHUD->SpectatorWidget)
        {
            OTHUD->SpectatorWidget->SetTargetNameText(PlayerName);
        }
    }
}

void AOTPlayerController::ClientShowGameResults_Implementation()
{
    if (OTHUD && OTHUD->CharacterOverlay)
    {
        OTHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
    }

    if (OTHUD && OTHUD->Announcement)
    {
        OTHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
    }

    if (OTHUD && OTHUD->SpectatorWidget)
    {
        OTHUD->SpectatorWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (OTHUD)
    {
        OTHUD->AddGameResultsWidget();
    }

    SetInputMode(FInputModeUIOnly());
    SetShowMouseCursor(true);
}

void AOTPlayerController::ClientShowAnnouncement_Implementation(EAnnouncementType Type, float ShowTime)
{
    this->ShowAnnouncement(Type, ShowTime);
}

void AOTPlayerController::OnMatchStateSet(FName State)
{
    MatchState = State;

    if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
    }
}

void AOTPlayerController::OnRep_MatchState()
{
    if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
    }
}

void AOTPlayerController::HandleMatchHasStarted()
{
    if (IsLocalPlayerController())
    {
        MatchStartTimestamp = GetWorld()->GetTimeSeconds();
    }

    if(!OTHUD)
        OTHUD = Cast<AOTHUD>(GetHUD());

    if (OTHUD)
    {
        if (OTHUD->CharacterOverlay == nullptr)
        {
            OTHUD->AddCharacterOverlay();
        }
        if (OTHUD->Announcement == nullptr)
        {
            OTHUD->AddAnnouncement();
            OTHUD->Announcement->ShowAnnouncement(EAnnouncementType::EANMT_MatchStart);
        }
    }

    HideLoadingUI();
}