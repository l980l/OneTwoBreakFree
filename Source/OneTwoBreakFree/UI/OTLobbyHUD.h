// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OTLobbyHUD.generated.h"

class UOTLobbyUI;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTLobbyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void AddLobbyWidget();

	UPROPERTY()
	TObjectPtr<UOTLobbyUI> LobbyWidget;

protected:
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UOTLobbyUI> LobbyUIClass;
    
};