// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OneTwoBreakFree/Types/AnnouncementType.h"
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
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION()
    void OnRep_MatchState();
    
    void OnMatchStateSet(FName State);
    void HandleMatchHasStarted();

    UPROPERTY(ReplicatedUsing = OnRep_MatchState)
    FName MatchState;

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
    void HideLoadingUI();
    void ShowAnnouncement(EAnnouncementType Type, float ShowTime = 3.f);
    void ToggleSettingUI();

    UFUNCTION(Client, Reliable)
    void ClientSetSpectatorUI();

    UFUNCTION(BlueprintCallable, Category = "Spectator")
    void EnterSpectatorMode();

    UFUNCTION(BlueprintCallable, Category = "Spectator")
    void SetSpectatingPlayerInfo(const FString& PlayerName);

    UFUNCTION(BlueprintCallable, Category = "Spectator")
    FORCEINLINE bool IsSpectating() const { return bIsSpectating; }

    UFUNCTION(Client, Reliable)
    void ClientShowAnnouncement(EAnnouncementType Type, float ShowTime = 3.f);

    UFUNCTION(Client, Reliable)
    void ClientShowGameResults();

private:
    UPROPERTY()
    TObjectPtr<class AOTHUD> OTHUD;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UOTLoadingUI> LoadingUIClass;

    UPROPERTY()
    TObjectPtr<class UOTLoadingUI> LoadingUI;

    float MatchStartTimestamp = 0.f;

    UPROPERTY(Replicated)
    bool bIsSpectating = false;

    UPROPERTY(Replicated)
    FString SpectatingPlayerName;
};
