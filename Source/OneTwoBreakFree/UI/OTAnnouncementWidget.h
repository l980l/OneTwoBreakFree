// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OneTwoBreakFree/Types/AnnouncementType.h"
#include "OTAnnouncementWidget.generated.h"

class UTextBlock;


/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTAnnouncementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AnnouncementTextFadeIn;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AnnouncementTextFadeOut;

public:
	void ShowAnnouncement(EAnnouncementType Type, float ShowTime = 3.f);

private:
	void ShowAnnouncementText(bool bShow);

	FTimerHandle HideWidgetTimerHandle;
};
