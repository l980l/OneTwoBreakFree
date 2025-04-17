// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCharacterOverlayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UOTCharacterOverlayWidget::SetHealthPercent(float HealthPercent)
{
    if (HealthBar)
    {
        const float ClampedHealth = FMath::Clamp(HealthPercent, 0.0f, 1.0f);
        HealthBar->SetPercent(ClampedHealth);
    }
}

void UOTCharacterOverlayWidget::SetHealthMarquee(bool bMarquee)
{
    if (HealthBar)
    {
        HealthBar->SetIsMarquee(bMarquee);
    }
}

void UOTCharacterOverlayWidget::SetStaminaPercent(float StaminaPercent)
{
    if (StaminaBar)
    {
        const float ClampedStamina = FMath::Clamp(StaminaPercent, 0.0f, 1.0f);
        StaminaBar->SetPercent(ClampedStamina);
    }
}

void UOTCharacterOverlayWidget::ShowStaminaWidget(bool bShow)
{
    if (StaminaBar)
    {
        if (bShow)
        {
            if (FadeInAnimation)
            {
                PlayAnimation(FadeInAnimation);
            }
        }
        else
        {
            if (FadeOutAnimation)
            {
                PlayAnimation(FadeOutAnimation);
            }
        }
    }
}

void UOTCharacterOverlayWidget::SetMatchTimeText(int32 Minutes, int32 Seconds)
{
    if (MatchTimeText)
    {
        const FString TimeText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        MatchTimeText->SetText(FText::FromString(TimeText));
    }
}

void UOTCharacterOverlayWidget::SetMatchTimeFromSeconds(float TotalSeconds)
{
    if (MatchTimeText)
    {
        const int32 Minutes = FMath::FloorToInt(TotalSeconds / 60.0f);
        const int32 Seconds = FMath::FloorToInt(TotalSeconds) % 60;

        const FString TimeText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        MatchTimeText->SetText(FText::FromString(TimeText));
    }
}

void UOTCharacterOverlayWidget::SetupKillerWidget()
{
    if (RifleText)
    {
        RifleText->SetVisibility(ESlateVisibility::Visible);
    }
    if (BazookaText)
    {
        BazookaText->SetVisibility(ESlateVisibility::Visible);
    }
    if (RifleAmmoText)
    {
        RifleAmmoText->SetVisibility(ESlateVisibility::Visible);
    }
    if (BazookaCooltimeProgress)
    {
        BazookaCooltimeProgress->SetVisibility(ESlateVisibility::Visible);
    }
}

void UOTCharacterOverlayWidget::SetRifleAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (RifleAmmoText)
    {
        const FString AmmoText = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
        RifleAmmoText->SetText(FText::FromString(AmmoText));
    }
}

void UOTCharacterOverlayWidget::SetBazookaPercent(float BazookaPercent)
{
    if (BazookaCooltimeProgress)
    {
        const float ClampedCooltime = FMath::Clamp(BazookaPercent, 0.0f, 1.0f);
        BazookaCooltimeProgress->SetPercent(ClampedCooltime);

        if (ClampedCooltime >= 1.f)
        {
            BazookaCooltimeProgress->SetRenderOpacity(1.f);
        }
        else
        {
            BazookaCooltimeProgress->SetRenderOpacity(0.2f);
        }
    }
}

void UOTCharacterOverlayWidget::FlashCrosshairRed()
{
    if (DotCrosshair)
    {
        DotCrosshair->SetColorAndOpacity(FLinearColor::Red);

        GetWorld()->GetTimerManager().SetTimer(
            CrosshairColorResetTimerHandle,
            [this]()
            {
                DotCrosshair->SetColorAndOpacity(FLinearColor::White);
            },
            0.7f,
            false
        );
    }
}
