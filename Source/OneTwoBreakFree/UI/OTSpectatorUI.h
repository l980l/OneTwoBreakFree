// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTSpectatorUI.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTSpectatorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TargetNameText;

public:
	void SetTargetNameText(const FString& NewTargetName);
};
