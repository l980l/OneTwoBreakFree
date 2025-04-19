// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OTHUD.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UOTCharacterOverlayWidget> CharacterOverlayClass;

	UPROPERTY()
	TObjectPtr<class UOTCharacterOverlayWidget> CharacterOverlay;

	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UOTAnnouncementWidget> AnnouncementClass;

	UPROPERTY()
	TObjectPtr<class UOTAnnouncementWidget> Announcement;

	void AddAnnouncement();
};
