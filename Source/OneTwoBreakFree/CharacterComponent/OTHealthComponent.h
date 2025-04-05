// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OTHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedSignature, UOTHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeathSignature, UOTHealthComponent*, HealthComp, AActor*, KilledActor, AActor*, KillerActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ONETWOBREAKFREE_API UOTHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
    UOTHealthComponent();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnDeathSignature OnDeath;

protected:
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UFUNCTION()
    void HandleDeath(AActor* KillerActor);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", Replicated)
    float MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Health")
    float Health;

    UPROPERTY(BlueprintReadOnly, Category = "Health")
    uint8 bIsDead : 1;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastOnDeath(AActor* KillerActor);
};