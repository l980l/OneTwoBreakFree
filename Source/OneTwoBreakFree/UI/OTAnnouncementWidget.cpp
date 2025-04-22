// Fill out your copyright notice in the Description page of Project Settings.


#include "OTAnnouncementWidget.h"
#include "Components/TextBlock.h"

void UOTAnnouncementWidget::ShowAnnouncement(EAnnouncementType Type, float ShowTime)
{
    FString AnnouncementMessage;
    FSlateColor TextColor;

    switch (Type)
    {
    case EAnnouncementType::EANMT_MatchStart:
        AnnouncementMessage = TEXT("Match Start!");
        TextColor = FSlateColor(FLinearColor(0.0f, 0.8f, 1.0f));
        break;
    case EAnnouncementType::EANMT_Survive:
        AnnouncementMessage = TEXT("You Survived!");
        TextColor = FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f)); 
        break;
    case EAnnouncementType::EANMT_Kill:
        AnnouncementMessage = TEXT("Kill!");
        TextColor = FSlateColor(FLinearColor(1.0f, 0.5f, 0.0f));
        break;
    case EAnnouncementType::EANMT_Dead:
        AnnouncementMessage = TEXT("You Die");
        TextColor = FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f));
        break;
    case EAnnouncementType::EANMT_MatchEnd:
        AnnouncementMessage = TEXT("Match End");
        TextColor = FSlateColor(FLinearColor(1.0f, 1.0f, 0.0f));
        break;
    default:
        AnnouncementMessage = TEXT("");
        break;
    }

    if (!AnnouncementMessage.IsEmpty() && AnnouncementText)
    {
        AnnouncementText->SetText(FText::FromString(AnnouncementMessage));
        AnnouncementText->SetColorAndOpacity(TextColor);

        if (IsAnimationPlaying(AnnouncementTextFadeIn))
        {
            StopAnimation(AnnouncementTextFadeIn);
        }
        if (IsAnimationPlaying(AnnouncementTextFadeOut))
        {
            StopAnimation(AnnouncementTextFadeOut);
        }

        ShowAnnouncementText(true);

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [this]() { ShowAnnouncementText(false); },
            ShowTime,
            false
        );
    }
}

void UOTAnnouncementWidget::ShowAnnouncementText(bool bShow)
{
    if (AnnouncementText)
    {
        if (bShow)
        {
            if (AnnouncementTextFadeIn)
            {
                PlayAnimation(AnnouncementTextFadeIn);
            }
        }
        else
        {
            if (AnnouncementTextFadeOut)
            {
                PlayAnimation(AnnouncementTextFadeOut);
            }
        }
    }
}
