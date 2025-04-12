// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OTRocket.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class ONETWOBREAKFREE_API AOTRocket : public AActor
{
	GENERATED_BODY()
	
public:	
	AOTRocket();

	void InitVelocity(const FVector& ShootDirection);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Destruction")
	TSubclassOf<class AGeometryCollectionActor> DestructibleWallClass;

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastExplosionEffects(const FVector& Location);

	void TriggerWallDestruction(const FVector& ImpactPoint);

	UFUNCTION(Server, Reliable)
	void ServerTriggerWallDestruction(const FVector& ImpactPoint);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTriggerWallDestruction(const FVector& ImpactPoint);

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	TObjectPtr<UParticleSystemComponent> TrailFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ExplosionRadius;
};
