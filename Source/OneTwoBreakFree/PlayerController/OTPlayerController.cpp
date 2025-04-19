// Fill out your copyright notice in the Description page of Project Settings.


#include "OTPlayerController.h"
#include "OneTwoBreakFree/UI/OTCharacterOverlayWidget.h"
#include "OneTwoBreakFree/UI/OTAnnouncementWidget.h"
#include "OneTwoBreakFree/UI/OTHUD.h"
#include "Kismet/GameplayStatics.h"
#include "OneTwoBreakFree/GameState/OTMatchGameState.h"
#include "OneTwoBreakFree/GameMode/OTMatchGameMode.h"
#include "Net/UnrealNetwork.h"

void AOTPlayerController::BeginPlay()
{
    Super::BeginPlay();

    OTHUD = Cast<AOTHUD>(GetHUD());

    SetInputMode(FInputModeGameOnly());
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
            OTHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}