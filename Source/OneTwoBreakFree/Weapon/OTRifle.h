// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTWeapon.h"
#include "OTRifle.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTRifle : public AOTWeapon
{
	GENERATED_BODY()

public:
	AOTRifle();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Fire()	override;
	virtual void Reload() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireEffects();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastImpactEffects(FVector_NetQuantize ImpactPoint, FVector_NetQuantize ImpactNormal);

	void FireLineTrace();
	void PlayFireEffects();
	void SetAmmoWidget();

protected:
	UFUNCTION()
	void OnRep_CurrentAmmo();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 MagazineCapacity;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ReloadTime;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	uint8 bCanFire : 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	uint8 bIsReloading : 1;

	float LastFireTime;

	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
};
