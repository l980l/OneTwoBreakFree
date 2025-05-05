// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OTGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    virtual void Init() override;
    virtual void OnStart() override;

    UPROPERTY()
    int32 PlayerCount;

    void SavePlayerName(const FUniqueNetIdRepl& PlayerId, const FString& PlayerName);
    bool GetSavedPlayerName(const FUniqueNetIdRepl& PlayerId, FString& OutPlayerName);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    float GetMasterVolume() const;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetBGMVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    float GetBGMVolume() const;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    float GetSFXVolume() const;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SaveAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void LoadAudioSettings();

private:
    UPROPERTY()
    TMap<FString, FString> PlayerNameMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundMix> MasterSoundMix;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> MasterSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> BGMSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> SFXSoundClass;

    float CurrentMasterVolume = 1.f;
    float CurrentBGMVolume = 1.f;
    float CurrentSFXVolume = 1.f;
};
