// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OTMatchGameState.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTMatchGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnRep_PCGRandomSeed();

public:
	UPROPERTY(ReplicatedUsing = OnRep_PCGRandomSeed, Transient)
	int32 PCGRandomSeed;

	UPROPERTY(EditDefaultsOnly, Category = "PCG")
	TSubclassOf<AActor> PCGBackroomsActorClass;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
