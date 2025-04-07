// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTCharacterBase.h"
#include "OTCitizenCharacter.generated.h"

class UOTHealthComponent;

UCLASS()
class ONETWOBREAKFREE_API AOTCitizenCharacter : public AOTCharacterBase
{
	GENERATED_BODY()

public:
    AOTCitizenCharacter(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UOTHealthComponent> HealthComponent;

protected:
    virtual void BeginPlay() override;

protected:
    UFUNCTION()
    virtual void OnHealthChanged(class UOTHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType);

    UFUNCTION()
    virtual void OnCharacterDeath(class UOTHealthComponent* HealthComp, AActor* KilledActor, AActor* KillerActor);

public:
    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsAlive() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetCurrentHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const;
};
