// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCitizenCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OneTwoBreakFree/CharacterComponent/OTHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "OTCharacterMovementComponent.h"

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

void AOTCitizenCharacter::OnHealthChanged(UOTHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType)
{
	// 체력 변화시 실행될 로직
	// 여기서는 아무것도 하지 않지만 필요에 따라 확장 가능
	// 
	// 애니메이션 재생, 사운드

	if (IsLocallyControlled())
	{
		// HUD 적용, 화면 효과
	}
}

void AOTCitizenCharacter::OnCharacterDeath(UOTHealthComponent* HealthComp, AActor* KilledActor, AActor* KillerActor)
{
	// 캐릭터가 죽었을 때 실행될 로직
	// 이동 기능 비활성화
	GetCharacterMovement()->DisableMovement();

	// 컨트롤러 분리 (서버에서만)
	if (HasAuthority() && GetController())
	{
		GetController()->UnPossess();
	}

	// 콜리전 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 사망 애니메이션

	// 래그돌 활성화 (시각적 효과)
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// 타이머로 일정 시간 후 캐릭터 제거 (서버에서만)
	if (HasAuthority())
	{
		SetLifeSpan(10.0f); // 10초 후 제거
	}

	if (IsLocallyControlled())
	{
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
