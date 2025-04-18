// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTCharacterOverlayWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTCharacterOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MatchTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DotCrosshair;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnimation;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnimation;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RifleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BazookaText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RifleAmmoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> BazookaCooltimeProgress;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HitMarker;

public:
	void SetHealthPercent(float HealthPercent);
	void SetHealthMarquee(bool bMarquee);
	void SetStaminaPercent(float StaminaPercent);
	void ShowStaminaWidget(bool bShow);
	void SetMatchTimeText(int32 Minutes, int32 Seconds);
	void SetMatchTimeFromSeconds(float TotalSeconds);
	void SetupKillerWidget();
	void SetRifleAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	void SetBazookaPercent(float BazookaPercent);
	void FlashCrosshairRed();
	void FlashHitMarker();

private:
	FTimerHandle CrosshairColorResetTimerHandle;
	FTimerHandle HitMarkerTimerHandle;
};
