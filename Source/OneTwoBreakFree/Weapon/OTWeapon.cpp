// Fill out your copyright notice in the Description page of Project Settings.


#include "OTWeapon.h"
#include "Kismet/GameplayStatics.h"

AOTWeapon::AOTWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetOwnerNoSee(true);
	WeaponMesh->bCastDynamicShadow = true;
	WeaponMesh->CastShadow = true;


	FirstPersonWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonWeaponMesh"));
	FirstPersonWeaponMesh->SetupAttachment(RootComponent);
	FirstPersonWeaponMesh->SetOnlyOwnerSee(true); 
	FirstPersonWeaponMesh->CastShadow = false;

	Damage = 20.0f;
	AttackRange = 1000.0f;
}

void AOTWeapon::PlaySound(EWeaponSoundType SoundType)
{
	USoundBase* Sound = nullptr;

	switch (SoundType)
	{
	case EWeaponSoundType::Fire:
		if (FireSound)
		{
			Sound = FireSound;
		}
		break;
	case EWeaponSoundType::Reload:
		if (ReloadSound)
		{
			Sound = ReloadSound;
		}
		break;
	case EWeaponSoundType::Equip:
		if (EquipSound)
		{
			Sound = EquipSound;
		}
		break;
	}

	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}
