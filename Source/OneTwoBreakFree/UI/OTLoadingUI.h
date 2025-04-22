// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTLoadingUI.generated.h"

class UTextBlock;
class UBorder;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTLoadingUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> LoadingBoarder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LoadingText;

public:
	void HideLoadingUI();
};
