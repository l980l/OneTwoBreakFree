// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OneTwoBreakFree/Types/CharacterRoleTypes.h"
#include "OTPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
    AOTPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    float DeathTime;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    float EscapeTime;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    int32 KillCount;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    EOTCharacterRole CharacterRole;

    FORCEINLINE bool IsKiller() const { return CharacterRole == EOTCharacterRole::ECR_Killer; }
    FORCEINLINE bool IsCitizen() const { return CharacterRole == EOTCharacterRole::ECR_Citizen; }
    FORCEINLINE bool HasEscaped() const { return EscapeTime > 0.0f; }
    FORCEINLINE bool IsDead() const { return DeathTime > 0.0f; }
};
