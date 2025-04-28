// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OTGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    virtual void Init() override;

    UPROPERTY()
    int32 PlayerCount;

    void SavePlayerName(const FUniqueNetIdRepl& PlayerId, const FString& PlayerName);
    bool GetSavedPlayerName(const FUniqueNetIdRepl& PlayerId, FString& OutPlayerName);

private:
    UPROPERTY()
    TMap<FString, FString> PlayerNameMap;
};
