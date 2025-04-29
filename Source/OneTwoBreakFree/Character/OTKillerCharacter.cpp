// Fill out your copyright notice in the Description page of Project Settings.


#include "OTKillerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OTCharacterMovementComponent.h"
#include "OneTwoBreakFree/CharacterComponent/OTWeaponComponent.h"
#include "OneTwoBreakFree/Weapon/OTWeapon.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"
#include "OneTwoBreakFree/GameState/OTMatchGameState.h"
#include "Kismet/GameplayStatics.h"

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

		if (WeaponComponent && PrimaryWeapon && SecondaryWeapon)
		{
			WeaponComponent->SetupWeapons(PrimaryWeapon, SecondaryWeapon);
			WeaponComponent->OnWeaponSwap.AddDynamic(this, &AOTKillerCharacter::PlaySwapMontage);
			PrimaryWeapon->OnWeaponFire.AddDynamic(this, &AOTKillerCharacter::PlayFireMontage);
			PrimaryWeapon->OnWeaponReload.AddDynamic(this, &AOTKillerCharacter::PlayReloadMontage);
			SecondaryWeapon->OnWeaponFire.AddDynamic(this, &AOTKillerCharacter::PlayFireMontage);
			SecondaryWeapon->OnWeaponReload.AddDynamic(this, &AOTKillerCharacter::PlayReloadMontage);
		}
	}
}

void AOTKillerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsLocallyControlled())
	{
		GetWorldTimerManager().SetTimer(SetupHUDTimerHandle, this, &AOTKillerCharacter::CheckAndSetup, 0.1f, true);
	}
}

void AOTKillerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsLocallyControlled())
	{
		GetWorldTimerManager().SetTimer(SetupHUDTimerHandle, this, &AOTKillerCharacter::CheckAndSetup, 0.1f, true);
	}
}

void AOTKillerCharacter::CheckAndSetup()
{
	if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
	{
		if (WeaponComponent->SetWeaponOwnerPlayerController(PC) && PC->GetHUD())
		{
			PC->SetupKillerWidget();
			PC->SetHUDHealthMarquee(true);
			GetWorldTimerManager().ClearTimer(SetupHUDTimerHandle);
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

void AOTKillerCharacter::PlayFireMontage()
{
	MulticastPlayFireMontage();
}

void AOTKillerCharacter::PlayReloadMontage()
{
	MulticastPlayReloadMontage();
}

void AOTKillerCharacter::PlaySwapMontage()
{
	MulticastPlaySwapMontage();
}

void AOTKillerCharacter::ClientPlayKillSound_Implementation()
{
	if (KillSound)
	{
		UGameplayStatics::PlaySound2D(this, KillSound);
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

void AOTKillerCharacter::MulticastPlayFireMontage_Implementation()
{
	if (IsLocallyControlled())
	{
		if (FireMontage_FP)
		{
			UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(FireMontage_FP);
			}
		}
	}

	else if (FireMontage_TP)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(FireMontage_TP);
		}
	}
}

void AOTKillerCharacter::MulticastPlayReloadMontage_Implementation()
{
	if (IsLocallyControlled())
	{
		if (ReloadMontage_FP)
		{
			UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(ReloadMontage_FP);
			}
		}
	}

	else if (ReloadMontage_TP)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadMontage_TP);
		}
	}
}

void AOTKillerCharacter::MulticastPlaySwapMontage_Implementation()
{
	if (IsLocallyControlled())
	{
		if (SwapMontage_FP)
		{
			UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(SwapMontage_FP);
			}
		}
	}

	else if (SwapMontage_TP)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(SwapMontage_TP);
		}
	}
}
