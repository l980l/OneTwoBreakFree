// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTCharacterBase.h"
#include "OTKillerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UOTWeaponComponent;
class AOTWeapon;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTKillerCharacter : public AOTCharacterBase
{
	GENERATED_BODY()

public:
	AOTKillerCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(Client, Reliable)
	void ClientPlayKillSound();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CheckAndSetup();
	void StartFire();
	void StartReload();
	void StartSwapWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOTWeaponComponent> WeaponComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AOTWeapon> PrimaryWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AOTWeapon> SecondaryWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SwapAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> WeaponMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage_FP;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ReloadMontage_FP;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwapMontage_FP;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage_TP;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ReloadMontage_TP;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwapMontage_TP;

private:
	UFUNCTION()
	void PlayFireMontage();

	UFUNCTION()
	void PlayReloadMontage();

	UFUNCTION()
	void PlaySwapMontage();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayFireMontage();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayReloadMontage();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySwapMontage();

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> KillSound;

	FTimerHandle SetupHUDTimerHandle;
};
