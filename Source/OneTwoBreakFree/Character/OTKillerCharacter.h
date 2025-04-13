// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTCharacterBase.h"
#include "OTKillerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UOTWeaponComponent;
class AOTWeapon;

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTKillerCharacter : public AOTCharacterBase
{
	GENERATED_BODY()

public:
	AOTKillerCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOTWeaponComponent> WeaponComponent;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CheckHUDAndSetupUI();
	void StartFire();
	void StartReload();
	void StartSwapWeapon();

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

private:
	FTimerHandle SetupHUDTimerHandle;
};
