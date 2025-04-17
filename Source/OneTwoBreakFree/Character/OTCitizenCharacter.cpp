// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCitizenCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OneTwoBreakFree/CharacterComponent/OTHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "OTCharacterMovementComponent.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"

AOTCitizenCharacter::AOTCitizenCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	HealthComponent = CreateDefaultSubobject<UOTHealthComponent>(TEXT("HealthComponent"));
	CharacterRole = EOTCharacterRole::ECR_Citizen;
}

void AOTCitizenCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 체력 컴포넌트 이벤트 바인딩
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AOTCitizenCharacter::OnCharacterDeath);
		HealthComponent->OnHealthChanged.AddDynamic(this, &AOTCitizenCharacter::OnHealthChanged);
	}
}

void AOTCitizenCharacter::OnHealthChanged(UOTHealthComponent* HealthComp, float Health, float HealthDelta)
{
	// 체력 변화시 실행될 로직
	// 여기서는 아무것도 하지 않지만 필요에 따라 확장 가능
	// 
	// 애니메이션 재생, 사운드

	if (IsLocallyControlled())
	{
		// HUD 적용, 화면 효과
		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			PC->SetHUDHealth(Health / GetMaxHealth());
			PC->ClientStartCameraShake(FireCameraShake, 5.0f);
		}
	}
}

void AOTCitizenCharacter::OnCharacterDeath(UOTHealthComponent* HealthComp, AActor* KilledActor, AActor* KillerActor)
{
	GetCharacterMovement()->DisableMovement();

	// 컨트롤러 분리 (서버에서만)
	/*if (HasAuthority() && GetController())
	{
		GetController()->UnPossess();
	}*/

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	if (HasAuthority())
	{
		SetLifeSpan(10.0f);
	}

	if (IsLocallyControlled())
	{
		// 사망 HUD
		// HUD 적용, 화면 효과
	}
}

bool AOTCitizenCharacter::IsAlive() const
{
	return HealthComponent && !(HealthComponent->bIsDead);
}

float AOTCitizenCharacter::GetCurrentHealth() const
{
	return HealthComponent ? HealthComponent->Health : 0.0f;
}

float AOTCitizenCharacter::GetMaxHealth() const
{
	return HealthComponent ? HealthComponent->MaxHealth : 0.0f;
}
