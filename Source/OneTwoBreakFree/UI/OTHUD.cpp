// Fill out your copyright notice in the Description page of Project Settings.


#include "OTHUD.h"
#include "GameFramework/PlayerController.h"
#include "OTCharacterOverlayWidget.h"
#include "OTAnnouncementWidget.h"

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
