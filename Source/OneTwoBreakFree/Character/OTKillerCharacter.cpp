// Fill out your copyright notice in the Description page of Project Settings.


#include "OTKillerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OTCharacterMovementComponent.h"
#include "OneTwoBreakFree/CharacterComponent/OTWeaponComponent.h"
#include "OneTwoBreakFree/Weapon/OTWeapon.h"

AOTKillerCharacter::AOTKillerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CharacterRole = EOTCharacterRole::ECR_Killer;

	WeaponComponent = CreateDefaultSubobject<UOTWeaponComponent>(TEXT("WeaponComponent"));
}

void AOTKillerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AOTWeapon* PrimaryWeapon = nullptr;
		if (PrimaryWeaponClass != nullptr)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			PrimaryWeapon = GetWorld()->SpawnActor<AOTWeapon>(PrimaryWeaponClass, FTransform(), SpawnParams);
		}

		AOTWeapon* SecondaryWeapon = nullptr;
		if (SecondaryWeaponClass != nullptr)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SecondaryWeapon = GetWorld()->SpawnActor<AOTWeapon>(SecondaryWeaponClass, FTransform(), SpawnParams);
		}

		// 무기 컴포넌트에 무기 설정
		if (WeaponComponent && PrimaryWeapon && SecondaryWeapon)
		{
			WeaponComponent->SetupWeapons(PrimaryWeapon, SecondaryWeapon);
		}
	}
}

void AOTKillerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AOTKillerCharacter::StartFire);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AOTKillerCharacter::StartReload);
		EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Started, this, &AOTKillerCharacter::StartSwapWeapon);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(WeaponMappingContext, 1);
		}
	}
}

void AOTKillerCharacter::StartFire()
{
	if (bIsKicking)
		return;

	if (WeaponComponent)
	{
		WeaponComponent->Fire();
	}
}

void AOTKillerCharacter::StartReload()
{
	if (bIsKicking)
		return;

	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
}

void AOTKillerCharacter::StartSwapWeapon()
{
	if (bIsKicking)
		return;

	if (WeaponComponent)
	{
		WeaponComponent->SwapWeapon();
	}
}
