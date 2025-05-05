// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/SpectatorPawn.h"
#include "OTSpectatorPawn.generated.h"

UCLASS()
class ONETWOBREAKFREE_API AOTSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	AOTSpectatorPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Spectator")
	TObjectPtr<AActor> SpectatingTarget;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> AvailableTargets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> NextTargetAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> PreviousTargetAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> ToggleFreeCamAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> OptionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> SpectatorMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	float TargetSwitchCooldown = 0.5f;

	bool bCanSwitchTarget = true;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Spectator")
	bool bFreeCameraMode = false;

	FTimerHandle SwitchTargetTimerHandle;

	void NextTarget(const FInputActionValue& Value);
	void PreviousTarget(const FInputActionValue& Value);
	void ToggleFreeCamera(const FInputActionValue& Value);
	void OptionPressed(const FInputActionValue& Value);

	void UpdateAvailableTargets();

	void SwitchToTarget(AActor* NewTarget);

	void SwitchToNextTarget();

	void SwitchToPreviousTarget();

	void EnableTargetSwitch();

	void UpdateTargetTracking(float DeltaTime);

	void ToggleFreeCameraMode();

private:
	UFUNCTION(Server, Reliable)
	void ServerSwitchToTarget(AActor* NewTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwitchToTarget(AActor* NewTarget);

	UFUNCTION(Server, Reliable)
	void ServerToggleFreeCameraMode(bool bNewFreeCameraMode);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastToggleFreeCameraMode(bool bNewFreeCameraMode);
};
