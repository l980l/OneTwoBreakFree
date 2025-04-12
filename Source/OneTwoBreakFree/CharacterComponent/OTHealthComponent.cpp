// Fill out your copyright notice in the Description page of Project Settings.


#include "OTHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "OneTwoBreakFree/Character/OTCitizenCharacter.h"

UOTHealthComponent::UOTHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);

    MaxHealth = 100.0f;
    Health = MaxHealth;
    bIsDead = false;
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

    OnHealthChanged.Broadcast(this, Health, -Damage);

    if (Health <= 0.0f && !bIsDead)
    {
        bIsDead = true;

        MulticastOnDeath(DamageCauser);
    }
}

void UOTHealthComponent::OnRep_Health(float OldHealth)
{
    float DeltaHealth = Health - OldHealth;
    OnHealthChanged.Broadcast(this, Health, DeltaHealth);
}

void UOTHealthComponent::MulticastOnDeath_Implementation(AActor* KillerActor)
{
    OnDeath.Broadcast(this, GetOwner(), KillerActor);
}
