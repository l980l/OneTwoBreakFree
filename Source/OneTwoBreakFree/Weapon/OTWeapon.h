// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OTWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadDelegate);

UENUM()
enum class EWeaponSoundType : uint8
{
	Fire,
	Reload,
	Equip,
	Max
};

UCLASS()
class ONETWOBREAKFREE_API AOTWeapon : public AActor
{
	GENERATED_BODY()

public:
	AOTWeapon();

public:	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire() {};

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload() {};

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlaySound(EWeaponSoundType SoundType);

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponFireDelegate OnWeaponFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponReloadDelegate OnWeaponReload;

protected:
	void SetOwnerPlayerController(class AOTPlayerController* NewPlayerController);
	
	TObjectPtr<class AOTPlayerController> OwnerPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> FirstPersonWeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> ReloadSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AttackRange;

	friend class UOTWeaponComponent;
};
