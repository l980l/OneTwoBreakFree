// Fill out your copyright notice in the Description page of Project Settings.


#include "OTHUD.h"
#include "GameFramework/PlayerController.h"
#include "OTCharacterOverlayWidget.h"
#include "OTAnnouncementWidget.h"
#include "OTSpectatorUI.h"
#include "OTGameResultsWidget.h"
#include "OTSettingUI.h"

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

void AOTHUD::AddSpectatorWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && SpectatorWidgetClass)
	{
		SpectatorWidget = CreateWidget<UOTSpectatorUI>(PlayerController, SpectatorWidgetClass);
		SpectatorWidget->AddToViewport();
	}
}

void AOTHUD::AddGameResultsWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && GameResultsWidgetClass)
	{
		GameResultsWidget = CreateWidget<UOTGameResultsWidget>(PlayerController, GameResultsWidgetClass);
		GameResultsWidget->AddToViewport();
	}
}

void AOTHUD::ToggleSettingUI()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && SettingUIClass)
	{
		if (SettingUI)
		{
			SettingUI->RemoveFromParent();

			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;

			SettingUI = nullptr;
		}
		else
		{
			SettingUI = CreateWidget<UOTSettingUI>(PlayerController, SettingUIClass);
			SettingUI->AddToViewport();

			PlayerController->SetInputMode(FInputModeGameAndUI());
			PlayerController->bShowMouseCursor = true;
		}
	}
}
