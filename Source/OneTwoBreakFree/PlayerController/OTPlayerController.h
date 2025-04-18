// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void SetHUDHealth(float HealthPercent);
    void SetHUDHealthMarquee(bool bMarquee);
    void SetHUDStamina(float StaminaPercent);
    void ShowHUDStamina(bool bShow);
    void SetHUDMatchTime(int32 Minutes, int32 Seconds);
    void SetHUDMatchTimeFromSeconds(float TotalSeconds);
    void SetupKillerWidget();
    void SetHUDRifleAmmo(int32 CurrentAmmo, int32 MaxAmmo);
    void SetHUDBazookaPercent(float BazookaPercent);
    void FlashCrosshairRed();
    void FlashHitMarker();
    
    UPROPERTY(ReplicatedUsing = OnRep_MatchState)
    FName MatchState;

    UFUNCTION()
    void OnRep_MatchState();

    void OnMatchStateSet(FName State);
    void HandleMatchHasStarted();

private:
    UPROPERTY()
    TObjectPtr<class AOTHUD> OTHUD;
};
