// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTCharacterBase.h"
#include "OTKillerCharacter.generated.h"

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartFire();
	void StartReload();
	void StartSwapWeapon();

	// 메인 무기 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AOTWeapon> PrimaryWeaponClass;

	// 보조 무기 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AOTWeapon> SecondaryWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> SwapAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> WeaponMappingContext;
};
