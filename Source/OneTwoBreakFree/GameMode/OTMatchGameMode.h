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
class AOTPortal;

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

    void CheckGameEndCondition();
    void EndGame();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
    int32 PortalCount = 4;

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

    UPROPERTY(EditDefaultsOnly, Category = "Portal")
    TSubclassOf<AOTPortal> PortalClass;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    float ResultsScreenDuration = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName LobbyLevelName = FName("Lobby");

private:
    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void OnMatchStateSet() override;

    void StartGame();
    void CheckAndStartGameIfReady();
    void CalculateMapBounds();
    void AssignPlayerRoles();
    TArray<FVector> FindSpawnLocations(int32 LocationCount);
    bool IsValidSpawnLocation(const FVector& Location, const TArray<FVector>& ExistingLocations);
    void TeleportPlayersToLocations(const TArray<FVector>& Locations);
    void SpawnPortalsAtLocations(const TArray<FVector>& Locations);
    void ShowResultsScreen();
    void ReturnToLobby();
    void BroadcastGameEndingSoon();

    bool bIsMapGenerated = false;

    FTimerHandle GameEndTimerHandle;
    bool bIsGameEnding = false;

    friend class AOTMatchGameState;
};
