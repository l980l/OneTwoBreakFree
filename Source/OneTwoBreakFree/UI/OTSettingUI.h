// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTSettingUI.generated.h"

class USlider;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTSettingUI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> BGMVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> SFXVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MasterVolumeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> BGMVolumeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SFXVolumeText;

    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnBGMVolumeChanged(float Value);

    UFUNCTION()
    void OnSFXVolumeChanged(float Value);

    void UpdateVolumeTexts();

private:
    UPROPERTY()
    TObjectPtr<class UOTGameInstance> GameInstance;
};
