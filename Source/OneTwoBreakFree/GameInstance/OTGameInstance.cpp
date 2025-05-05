// Fill out your copyright notice in the Description page of Project Settings.


#include "OTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

void UOTGameInstance::Init()
{
    Super::Init();

    PlayerCount = 0;

    CurrentMasterVolume = 1.f;
    CurrentBGMVolume = 1.f;
    CurrentSFXVolume = 1.f;
}

void UOTGameInstance::OnStart()
{
    Super::OnStart();
    LoadAudioSettings();
}

void UOTGameInstance::SavePlayerName(const FUniqueNetIdRepl& PlayerId, const FString& PlayerName)
{
    if (PlayerId.IsValid())
    {
        PlayerNameMap.Add(PlayerId->ToString(), PlayerName);
    }
}

bool UOTGameInstance::GetSavedPlayerName(const FUniqueNetIdRepl& PlayerId, FString& OutPlayerName)
{
    if (PlayerId.IsValid())
    {
        FString* FoundName = PlayerNameMap.Find(PlayerId->ToString());
        if (FoundName)
        {
            OutPlayerName = *FoundName;
            return true;
        }
    }
    return false;
}

void UOTGameInstance::SetMasterVolume(float Volume)
{
    if (!MasterSoundMix)
        return;

    float ClampedVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

    if (MasterSoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MasterSoundClass, ClampedVolume);
        UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);
    }

    CurrentMasterVolume = ClampedVolume;

    SaveAudioSettings();
}

void UOTGameInstance::SetBGMVolume(float Volume)
{
    if (!MasterSoundMix)
        return;

    float ClampedVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

    if (BGMSoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, BGMSoundClass, ClampedVolume);
        UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);
    }

    CurrentBGMVolume = ClampedVolume;

    SaveAudioSettings();
}

void UOTGameInstance::SetSFXVolume(float Volume)
{
    if (!MasterSoundMix)
        return;

    float ClampedVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

    if (SFXSoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, SFXSoundClass, ClampedVolume);
        UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);
    }

    CurrentSFXVolume = ClampedVolume;

    SaveAudioSettings();
}

float UOTGameInstance::GetMasterVolume() const
{
    return CurrentMasterVolume;
}

float UOTGameInstance::GetBGMVolume() const
{
    return CurrentBGMVolume;
}

float UOTGameInstance::GetSFXVolume() const
{
    return CurrentSFXVolume;
}

void UOTGameInstance::SaveAudioSettings()
{
    const FString Section = TEXT("AudioSettings");
    const FString ConfigFile = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Config/AudioSettings.ini"));

    GConfig->SetFloat(*Section, TEXT("MasterVolume"), CurrentMasterVolume, ConfigFile);
    GConfig->SetFloat(*Section, TEXT("BGMVolume"), CurrentBGMVolume, ConfigFile);
    GConfig->SetFloat(*Section, TEXT("SFXVolume"), CurrentSFXVolume, ConfigFile);

    GConfig->Flush(false, ConfigFile);
}

void UOTGameInstance::LoadAudioSettings()
{
    const FString Section = TEXT("AudioSettings");
    const FString ConfigFile = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Config/AudioSettings.ini"));

    float LoadedMasterVolume = 1.0f;
    float LoadedBGMVolume = 1.0f;
    float LoadedSFXVolume = 1.0f;

    GConfig->GetFloat(*Section, TEXT("MasterVolume"), LoadedMasterVolume, ConfigFile);
    GConfig->GetFloat(*Section, TEXT("BGMVolume"), LoadedBGMVolume, ConfigFile);
    GConfig->GetFloat(*Section, TEXT("SFXVolume"), LoadedSFXVolume, ConfigFile);

    SetMasterVolume(LoadedMasterVolume);
    SetBGMVolume(LoadedBGMVolume);
    SetSFXVolume(LoadedSFXVolume);
}