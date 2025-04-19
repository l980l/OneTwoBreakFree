// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTLobbyPlayerItem.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTLobbyPlayerItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void SetPlayerInfo(const FString& InPlayerName, bool bInIsReady, bool bInIsHost);

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* PlayerNameText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UImage* ReadyStatusIcon;
};
