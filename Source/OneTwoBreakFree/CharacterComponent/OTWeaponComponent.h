// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OTWeaponComponent.generated.h"

class AOTWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ONETWOBREAKFREE_API UOTWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOTWeaponComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	void Fire();
	void Reload();
	void SwapWeapon();
	void SetWeaponOwnerPlayerController(class AOTPlayerController* NewPlayerController);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetupWeapons(AOTWeapon* PrimaryWeapon, AOTWeapon* SecondaryWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AOTWeapon* GetCurrentWeapon() const { return Weapons[CurrentWeaponIndex]; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }

protected:
	void SetupFirstPersonWeaponMeshes();
	void UpdateWeaponVisibility();
	void ApplyWeaponSwapEffects();

	UFUNCTION()
	void OnRep_Weapons();

	UFUNCTION()
	void OnRepCurrentWeaponIndex();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(Server, Reliable)
	void ServerSwapWeapon();

	UPROPERTY(ReplicatedUsing = OnRep_Weapons, BlueprintReadOnly, Category = "Weapon")
	TArray<TObjectPtr<AOTWeapon>> Weapons;

	UPROPERTY(ReplicatedUsing = OnRepCurrentWeaponIndex, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	uint8 bIsSwapping : 1;

	FTimerHandle SwapTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float SwapTime;
};
