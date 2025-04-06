// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCharacterOverlayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UOTCharacterOverlayWidget::SetHealthPercent(float HealthPercent)
{
    if (HealthBar)
    {
        const float ClampedHealth = FMath::Clamp(HealthPercent, 0.0f, 1.0f);
        HealthBar->SetPercent(ClampedHealth);
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
