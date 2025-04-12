// Fill out your copyright notice in the Description page of Project Settings.


#include "OTWeaponComponent.h"
#include "OneTwoBreakFree/Weapon/OTWeapon.h"
#include "OneTwoBreakFree/Character/OTCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

UOTWeaponComponent::UOTWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	WeaponSocketName = "WeaponSocket";
	SwapTime = 0.5f;
	CurrentWeaponIndex = 0;
	bIsSwapping = false;
	Weapons.SetNum(2);
}

void UOTWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOTWeaponComponent, Weapons);
	DOREPLIFETIME(UOTWeaponComponent, CurrentWeaponIndex);
	DOREPLIFETIME(UOTWeaponComponent, bIsSwapping);
}

void UOTWeaponComponent::SetupWeapons(AOTWeapon* PrimaryWeapon, AOTWeapon* SecondaryWeapon)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	AOTCharacterBase* BaseCharacter = Cast<AOTCharacterBase>(OwnerCharacter);
	if (!BaseCharacter)
	{
		return;
	}

	if (PrimaryWeapon)
	{
		PrimaryWeapon->SetOwner(GetOwner());

		PrimaryWeapon->AttachToComponent(BaseCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

		if (BaseCharacter->FirstPersonMesh)
		{
			PrimaryWeapon->FirstPersonWeaponMesh->AttachToComponent(BaseCharacter->FirstPersonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}

		Weapons[0] = PrimaryWeapon;
	}

	if (SecondaryWeapon)
	{
		SecondaryWeapon->SetOwner(GetOwner());

		SecondaryWeapon->AttachToComponent(BaseCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

		if (BaseCharacter->FirstPersonMesh)
		{
			SecondaryWeapon->FirstPersonWeaponMesh->AttachToComponent(BaseCharacter->FirstPersonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}

		SecondaryWeapon->WeaponMesh->SetHiddenInGame(true);
		SecondaryWeapon->FirstPersonWeaponMesh->SetHiddenInGame(true);

		Weapons[1] = SecondaryWeapon;
	}

	CurrentWeaponIndex = 0;

	UpdateWeaponVisibility();
}

void UOTWeaponComponent::OnRep_Weapons()
{
	SetupFirstPersonWeaponMeshes();

	UpdateWeaponVisibility();
}

void UOTWeaponComponent::OnRepCurrentWeaponIndex()
{
	ApplyWeaponSwapEffects();
}

void UOTWeaponComponent::SetupFirstPersonWeaponMeshes()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	AOTCharacterBase* BaseCharacter = Cast<AOTCharacterBase>(OwnerCharacter);
	if (!BaseCharacter || !BaseCharacter->FirstPersonMesh)
	{
		return;
	}

	// 로컬 플레이어인 경우에만 1인칭 무기 메시 설정
	if (!OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	for (int32 i = 0; i < Weapons.Num(); ++i)
	{
		AOTWeapon* Weapon = Weapons[i];
		if (Weapon)
		{
			Weapon->FirstPersonWeaponMesh->AttachToComponent(BaseCharacter->FirstPersonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

			Weapon->WeaponMesh->SetOwnerNoSee(true);
			Weapon->FirstPersonWeaponMesh->SetOnlyOwnerSee(true);
		}
	}
}

void UOTWeaponComponent::UpdateWeaponVisibility()
{
	for (int32 i = 0; i < Weapons.Num(); ++i)
	{
		if (Weapons[i])
		{
			Weapons[i]->WeaponMesh->SetHiddenInGame(true);
			Weapons[i]->FirstPersonWeaponMesh->SetHiddenInGame(true);
		}
	}

	if (Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->WeaponMesh->SetHiddenInGame(false);
		Weapons[CurrentWeaponIndex]->FirstPersonWeaponMesh->SetHiddenInGame(false);
	}
}

void UOTWeaponComponent::ApplyWeaponSwapEffects()
{
	UpdateWeaponVisibility();

	// 2. 스왑 사운드 재생
	if (Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->PlaySound(EWeaponSoundType::Equip);
	}
	// 3. 무기 교체 애니메이션 재생 (캐릭터 애니메이션 시스템에 구현 필요)
}

void UOTWeaponComponent::Fire()
{
	if (bIsSwapping)
	{
		return;
	}

	AOTWeapon* CurrentWeapon = GetCurrentWeapon();
	if (CurrentWeapon)
	{
		// 로컬에서 발사 처리. 이때는 이펙트 및 사운드만 처리함. 
		CurrentWeapon->Fire();

		if (!GetOwner()->HasAuthority())
		{
			ServerFire();
		}
	}
}

void UOTWeaponComponent::ServerFire_Implementation()
{
	if (bIsSwapping)
	{
		return;
	}

	AOTWeapon* CurrentWeapon = GetCurrentWeapon();
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}

void UOTWeaponComponent::Reload()
{
	if (bIsSwapping)
	{
		return;
	}

	// 현재 무기 재장전
	AOTWeapon* CurrentWeapon = GetCurrentWeapon();
	if (CurrentWeapon)
	{
		// 로컬에서 재장전 처리
		CurrentWeapon->Reload();

		// 서버 요청
		if (!GetOwner()->HasAuthority()) // 클라이언트에서만 서버에 요청
		{
			ServerReload();
		}
	}
}

void UOTWeaponComponent::ServerReload_Implementation()
{
	if (bIsSwapping)
	{
		return;
	}

	// 서버에서 재장전 처리
	AOTWeapon* CurrentWeapon = GetCurrentWeapon();
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void UOTWeaponComponent::SwapWeapon()
{
	if (bIsSwapping)
	{
		return;
	}

	if (Weapons.Num() < 2 || !Weapons[0] || !Weapons[1])
	{
		return;
	}

	// 로컬에서 먼저 스왑 상태 설정
	bIsSwapping = true;

	if (!GetOwner()->HasAuthority())
	{
		ServerSwapWeapon();
	}
	else
	{
		int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
		CurrentWeaponIndex = NextWeaponIndex;

		ApplyWeaponSwapEffects();

		GetWorld()->GetTimerManager().SetTimer(
			SwapTimerHandle,
			[this]()
			{
				bIsSwapping = false;
			},
			SwapTime,
			false
		);
	}
}

void UOTWeaponComponent::ServerSwapWeapon_Implementation()
{
	if (bIsSwapping)
	{
		return;
	}

	if (Weapons.Num() < 2 || !Weapons[0] || !Weapons[1])
	{
		return;
	}

	bIsSwapping = true;

	int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
	CurrentWeaponIndex = NextWeaponIndex;

	ApplyWeaponSwapEffects();

	GetWorld()->GetTimerManager().SetTimer(
		SwapTimerHandle,
		[this]()
		{
			bIsSwapping = false;
		},
		SwapTime,
		false
	);
}
