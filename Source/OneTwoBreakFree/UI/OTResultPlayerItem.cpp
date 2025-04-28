// Fill out your copyright notice in the Description page of Project Settings.


#include "OTResultPlayerItem.h"
#include "OneTwoBreakFree/PlayerState/OTPlayerState.h"
#include "Components/TextBlock.h"

void UOTResultPlayerItem::SetPlayerInfo(AOTPlayerState* PlayerState)
{
	if (RoleText)
	{
		FString RoleString;
		switch (PlayerState->CharacterRole)
		{
		case EOTCharacterRole::ECR_Killer:
			RoleString = TEXT("Killer");
			break;
		case EOTCharacterRole::ECR_Citizen:
			RoleString = TEXT("Citizen");
			break;
		}
		RoleText->SetText(FText::FromString(RoleString));
	}
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
	}
	if (StateText)
	{
		FString StateString;
		if (PlayerState->HasEscaped())
		{
			StateString = TEXT("Survived!");
		}
		else if (PlayerState->IsDead())
		{
			StateString = TEXT("Dead");
		}
		else
		{
			StateString = TEXT("-");
		}
		StateText->SetText(FText::FromString(StateString));
	}
	if (ResultText)
	{
		FString ResultString;
		if (PlayerState->IsKiller())
		{
			ResultString = FString::Printf(TEXT("Kill : %d"), PlayerState->KillCount);
		}
		else
		{
			if (PlayerState->HasEscaped())
			{
				ResultString = FString::Printf(TEXT("Escape : %.2f"), PlayerState->EscapeTime);
			}
			else if (PlayerState->IsDead())
			{
				ResultString = FString::Printf(TEXT("Dead : %.2f"), PlayerState->DeathTime);
			}
		}
		ResultText->SetText(FText::FromString(ResultString));
	}
}
