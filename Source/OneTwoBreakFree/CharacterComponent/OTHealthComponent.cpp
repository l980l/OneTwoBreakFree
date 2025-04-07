// Fill out your copyright notice in the Description page of Project Settings.


#include "OTHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "OneTwoBreakFree/Character/OTCitizenCharacter.h"

UOTHealthComponent::UOTHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    MaxHealth = 100.0f;
    Health = MaxHealth;
    bIsDead = false;

    SetIsReplicatedByDefault(true);
}

void UOTHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwnerRole() == ROLE_Authority)
    {
        AActor* MyOwner = GetOwner();
        if (MyOwner)
        {
            MyOwner->OnTakeAnyDamage.AddDynamic(this, &UOTHealthComponent::HandleTakeAnyDamage);
        }
    }
}

void UOTHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UOTHealthComponent, MaxHealth);
    DOREPLIFETIME(UOTHealthComponent, Health);
    DOREPLIFETIME(UOTHealthComponent, bIsDead);
}

void UOTHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || bIsDead)
    {
        return;
    }

    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

    OnHealthChanged.Broadcast(this, Health, -Damage, DamageType);

    if (Health <= 0.0f && !bIsDead)
    {
        bIsDead = true;

        OnDeath.Broadcast(this, GetOwner(), DamageCauser);

        MulticastOnDeath(DamageCauser);
    }
}

void UOTHealthComponent::HandleDeath(AActor* KillerActor)
{
    bIsDead = true;

    // 게임모드를 통해, 플레이어 사망에 대한 GameState, PlayerState 처리. 

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        if (OwnerCharacter->GetController())
        {
            OwnerCharacter->GetController()->UnPossess();
        }

        OwnerCharacter->SetLifeSpan(10.0f);
    }

    MulticastOnDeath(KillerActor);
}

void UOTHealthComponent::MulticastOnDeath_Implementation(AActor* KillerActor)
{
    if (Cast<ACharacter>(GetOwner()) && Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        // HUD
    }

    // 사망 애니메이션 재생
    // 시각, 청각 부분 처리. 캐릭터에 대한 처리는 캐릭터에서 하며, 델리게이트를 통해 호출한다. 
}
