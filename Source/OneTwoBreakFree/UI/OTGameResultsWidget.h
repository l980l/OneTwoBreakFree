// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTGameResultsWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTGameResultsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UVerticalBox* PlayerListBox;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* TimerText;

protected:
	virtual void NativeConstruct() override;
	void SetupResultsScreen();
	void UpdateTimerText();

	class UOTResultPlayerItem* CreatePlayerItemWidget(class AOTPlayerState* PlayerState);

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UOTResultPlayerItem> PlayerItemWidgetClass;

	FTimerHandle TimerUpdateHandle;
	float RemainingTime;
};
