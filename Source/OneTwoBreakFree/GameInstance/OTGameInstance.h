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
};
