// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OneTwoBreakFree/Character/OTCharacterBase.h"
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

    UFUNCTION()
    void OnPCGGenerationCompleted(UPCGComponent* InPCGComponent);

    UFUNCTION(BlueprintCallable, Category = "PCG")
    void StartPCGMapGeneration();

    UPROPERTY(EditDefaultsOnly, Category = "PCG")
    TSubclassOf<AActor> PCGBackroomsActorClass;

    UPROPERTY(Transient)
    TObjectPtr<AActor> PCGBackroomsActor;

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

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Game")
    TObjectPtr<APlayerController> KillerPlayerController;

    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSubclassOf<AOTCharacterBase> KillerCharacterClass;

    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSubclassOf<AOTCharacterBase> CitizenCharacterClass;

private:
    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void OnMatchStateSet() override;

    void StartGame();
    void CheckAndStartGameIfReady();
    void CalculateMapBounds();
    void AssignPlayerRoles();
    TArray<FVector> FindPlayerSpawnLocations(int32 CountPlayers);
    bool IsValidSpawnLocation(const FVector& Location, const TArray<FVector>& ExistingLocations);
    void TeleportPlayersToLocations(const TArray<FVector>& Locations);

    bool bIsMapGenerated = false;
};
