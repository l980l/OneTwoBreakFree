// Fill out your copyright notice in the Description page of Project Settings.


#include "OTHUD.h"
#include "GameFramework/PlayerController.h"
#include "OTCharacterOverlayWidget.h"
#include "OTAnnouncementWidget.h"

void AOTHUD::DrawHUD()
{
	Super::DrawHUD();

	// 크로스헤어 적용. 다만 무기를 들고 있는 경우에만 적용해야 한다. 
}

void AOTHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UOTCharacterOverlayWidget>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AOTHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UOTAnnouncementWidget>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}
