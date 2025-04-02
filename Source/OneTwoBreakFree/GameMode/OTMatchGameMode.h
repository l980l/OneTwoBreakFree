// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OTMatchGameMode.generated.h"

/**
 * 
 */
class UPCGComponent;

UCLASS()
class ONETWOBREAKFREE_API AOTMatchGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AOTMatchGameMode();

    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;

    UFUNCTION()
    void OnPCGGenerationCompleted(UPCGComponent* InPCGComponent);

    UFUNCTION(BlueprintCallable, Category = "PCG")
    void StartPCGMapGeneration();

    UPROPERTY(EditDefaultsOnly, Category = "PCG")
    TSubclassOf<AActor> PCGBackroomsActorClass;

    UPROPERTY(Transient)
    TObjectPtr<AActor> PCGBackroomsActor;

    // 플레이어 스폰 관련 설정
    UPROPERTY(EditDefaultsOnly, Category = "PlayerSpawn")
    float MinDistanceBetweenPlayers = 1000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "PlayerSpawn")
    float MinDistanceFromWalls = 50.0f;

    UPROPERTY(EditDefaultsOnly, Category = "PlayerSpawn")
    int32 MaxSpawnAttempts = 100;

    UPROPERTY(EditDefaultsOnly, Category = "PlayerSpawn")
    float PlayerSpawnHeight = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
    int32 MaxPlayers = 4;

    UPROPERTY(Transient)
    FVector MapMin;

    UPROPERTY(Transient)
    FVector MapMax;

protected:
    UFUNCTION()
    void StartGame();
    void CheckAndStartGameIfReady();

    void CalculateMapBounds();

    TArray<FVector> FindPlayerSpawnLocations(int32 CountPlayers);

    bool IsValidSpawnLocation(const FVector& Location, const TArray<FVector>& ExistingLocations);

    void TeleportPlayersToLocations(const TArray<FVector>& Locations);

    // 상태 변수
    bool bIsMapGenerated = false;
    bool bIsGameStarted = false;
};
