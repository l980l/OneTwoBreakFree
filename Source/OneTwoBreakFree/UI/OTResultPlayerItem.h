// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OTResultPlayerItem.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTResultPlayerItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetPlayerInfo(class AOTPlayerState* PlayerState);

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* RoleText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* PlayerNameText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* StateText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* ResultText;
};
