// Fill out your copyright notice in the Description page of Project Settings.


#include "OTSpectatorUI.h"
#include "Components/TextBlock.h"

void UOTSpectatorUI::SetTargetNameText(const FString& NewTargetName)
{
	if (TargetNameText)
	{
		TargetNameText->SetText(FText::FromString(NewTargetName));
	}
}
