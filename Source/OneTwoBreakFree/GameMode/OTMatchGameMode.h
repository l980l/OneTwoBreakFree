// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OTMatchGameMode.generated.h"

/**
 * 
 */
class UPCGComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPCGMapGenerated);

UCLASS()
class ONETWOBREAKFREE_API AOTMatchGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AOTMatchGameMode();

    virtual void BeginPlay() override;

    UFUNCTION()
    void OnPCGGenerationCompleted(UPCGComponent* InPCGComponent);

    UFUNCTION(BlueprintCallable, Category = "PCG")
    TArray<FTransform> FindPlayerSpawnPoints(int32 PlayerCount);

    UFUNCTION(BlueprintCallable, Category = "PCG")
    void TeleportPlayersToSpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "PCG")
    void StartPCGMapGeneration();

    UPROPERTY(BlueprintAssignable, Category = "PCG")
    FOnPCGMapGenerated OnPCGMapGenerated;

    UPROPERTY(EditDefaultsOnly, Category = "PCG")
    TSubclassOf<AActor> PCGBackroomsActorClass;

    UPROPERTY(Transient)
    TObjectPtr<AActor> PCGBackroomsActor;

    bool bIsMapGenerated = false;
};
