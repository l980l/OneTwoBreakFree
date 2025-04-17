// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTWeapon.h"
#include "OTBazooka.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTBazooka : public AOTWeapon
{
	GENERATED_BODY()

public:
	AOTBazooka();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Fire()	override;

private:
	UFUNCTION(Server, Reliable)
	void ServerFireProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireEffects();

	UFUNCTION()
	void OnCooldownComplete();

	void PlayFireEffects();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AOTRocket> RocketClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> MuzzleFlash;

	// 현재 탄약 (0 또는 1)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;

	// 최대 탄약 (항상 1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 MaxAmmo;

	// 쿨타임 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Cooldown;

	// 남은 쿨타임 (초)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	float CooldownRemaining;

	// 쿨타임 타이머 핸들
	FTimerHandle CooldownTimerHandle;

	// 발사 가능 여부
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	bool bCanFire;

	// 쿨다운 진행 중 여부
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	bool bIsOnCooldown;
};
