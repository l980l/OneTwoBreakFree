// Fill out your copyright notice in the Description page of Project Settings.


#include "OTSettingUI.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "OneTwoBreakFree/GameInstance/OTGameInstance.h"

void UOTSettingUI::NativeConstruct()
{
    Super::NativeConstruct();

    GameInstance = Cast<UOTGameInstance>(GetGameInstance());

    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UOTSettingUI::OnMasterVolumeChanged);

        if (GameInstance)
        {
            MasterVolumeSlider->SetValue(GameInstance->GetMasterVolume());
        }
    }

    if (BGMVolumeSlider)
    {
        BGMVolumeSlider->OnValueChanged.AddDynamic(this, &UOTSettingUI::OnBGMVolumeChanged);

        if (GameInstance)
        {
            BGMVolumeSlider->SetValue(GameInstance->GetBGMVolume());
        }
    }

    if (SFXVolumeSlider)
    {
        SFXVolumeSlider->OnValueChanged.AddDynamic(this, &UOTSettingUI::OnSFXVolumeChanged);

        if (GameInstance)
        {
            SFXVolumeSlider->SetValue(GameInstance->GetSFXVolume());
        }
    }

    UpdateVolumeTexts();
}

void UOTSettingUI::OnMasterVolumeChanged(float Value)
{
    if (GameInstance)
    {
        GameInstance->SetMasterVolume(Value);
    }

    UpdateVolumeTexts();
}

void UOTSettingUI::OnBGMVolumeChanged(float Value)
{
    if (GameInstance)
    {
        GameInstance->SetBGMVolume(Value);
    }

    UpdateVolumeTexts();
}

void UOTSettingUI::OnSFXVolumeChanged(float Value)
{
    if (GameInstance)
    {
        GameInstance->SetSFXVolume(Value);
    }

    UpdateVolumeTexts();
}

void UOTSettingUI::UpdateVolumeTexts()
{
    if (!GameInstance)
    {
        return;
    }

    if (MasterVolumeText)
    {
        int32 Percent = FMath::RoundToInt(GameInstance->GetMasterVolume() * 100.0f);
        MasterVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percent)));
    }

    if (BGMVolumeText)
    {
        int32 Percent = FMath::RoundToInt(GameInstance->GetBGMVolume() * 100.0f);
        BGMVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percent)));
    }

    if (SFXVolumeText)
    {
        int32 Percent = FMath::RoundToInt(GameInstance->GetSFXVolume() * 100.0f);
        SFXVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percent)));
    }
}