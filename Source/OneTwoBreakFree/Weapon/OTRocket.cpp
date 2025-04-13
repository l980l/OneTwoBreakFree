// Fill out your copyright notice in the Description page of Project Settings.


#include "OTRocket.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h"

AOTRocket::AOTRocket()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentHit.AddDynamic(this, &AOTRocket::OnHit);
	RootComponent = CollisionComponent;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;	
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 1.0f;

	TrailFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailFX"));
	TrailFX->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;

	Damage = 100.0f;
	ExplosionRadius = 300.0f;

}

void AOTRocket::InitVelocity(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

void AOTRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner() || OtherActor->GetOwner() == GetOwner())
		return;

	if (GetLocalRole() == ROLE_Authority)
	{
		const FVector ImpactLocation = Hit.ImpactPoint;

		MulticastExplosionEffects(ImpactLocation);

		TriggerWallDestruction(ImpactLocation);

		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		IgnoredActors.Add(GetInstigator());

		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			Damage,
			ImpactLocation,
			ExplosionRadius,
			nullptr,
			IgnoredActors,
			this,
			GetInstigatorController()
		);

		SetLifeSpan(1.f);
	}
}

void AOTRocket::MulticastExplosionEffects_Implementation(const FVector& Location)
{
	ProjectileMesh->SetVisibility(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovement->StopMovementImmediately();

	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, Location);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, Location);
	}
}

void AOTRocket::TriggerWallDestruction(const FVector& ImpactPoint)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        // 영향 범위 내의 모든 액터 찾기
        TArray<AActor*> OverlappingActors;
        UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            ImpactPoint,
            ExplosionRadius,
            TArray<TEnumAsByte<EObjectTypeQuery>>(),
            AStaticMeshActor::StaticClass(),
            TArray<AActor*>(),
            OverlappingActors
        );

        if (!DestructibleWallClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("DestructibleWallClass is not set in Rocket"));
            return;
        }

        // 범위 내의 모든 파괴 가능한 벽 처리
        for (AActor* Actor : OverlappingActors)
        {
            AStaticMeshActor* WallActor = Cast<AStaticMeshActor>(Actor);
            if (WallActor && WallActor->Tags.Contains("DestructibleWall"))
            {
                const FVector WallLocation = WallActor->GetActorLocation();
                const FRotator WallRotation = WallActor->GetActorRotation();

                // 서버에서는 직접 호출, 클라이언트에서는 서버로 요청
                if (GetLocalRole() == ROLE_Authority)
                {
                    MulticastTriggerWallDestruction(ImpactPoint);
                }
                else
                {
                    ServerTriggerWallDestruction(ImpactPoint);
                }
            }
        }
    }
}

void AOTRocket::ServerTriggerWallDestruction_Implementation(const FVector& ImpactPoint)
{
    MulticastTriggerWallDestruction(ImpactPoint);
}

void AOTRocket::MulticastTriggerWallDestruction_Implementation(const FVector& ImpactPoint)
{
    // 영향 범위 내의 모든 액터 찾기
    TArray<AActor*> OverlappingActors;
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        ImpactPoint,
        ExplosionRadius,
        TArray<TEnumAsByte<EObjectTypeQuery>>(),
        AStaticMeshActor::StaticClass(),
        TArray<AActor*>(),
        OverlappingActors
    );

    for (AActor* Actor : OverlappingActors)
    {
        AStaticMeshActor* WallActor = Cast<AStaticMeshActor>(Actor);
        if (WallActor && WallActor->Tags.Contains("DestructibleWall"))
        {
            const FVector WallLocation = WallActor->GetActorLocation();
            const FRotator WallRotation = WallActor->GetActorRotation();

            WallActor->SetActorHiddenInGame(true);
            WallActor->SetActorEnableCollision(false);

            FActorSpawnParameters SpawnParams;
            AGeometryCollectionActor* DestructibleWall = GetWorld()->SpawnActor<AGeometryCollectionActor>(
                DestructibleWallClass,
                WallLocation,
                WallRotation,
                SpawnParams
            );

            if (DestructibleWall)
            {
                DestructibleWall->SetActorScale3D(WallActor->GetActorScale3D());

                UGeometryCollectionComponent* GeoComp = DestructibleWall->GetGeometryCollectionComponent();
                if (GeoComp)
                {
                    FVector ImpactDirection = (WallLocation - ImpactPoint).GetSafeNormal();

                    // 약간의 딜레이 후 파괴 (렌더링 보장)
                    FTimerHandle BreakTimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        BreakTimerHandle,
                        [GeoComp, ImpactDirection, ImpactPoint, WallLocation]()
                        {
                            const float ForceMultiplier = 2000000.0f;
                            GeoComp->AddImpulseAtLocation(ImpactDirection * ForceMultiplier, ImpactPoint);
                        },
                        0.3f,
                        false
                    );

                    // 일정 시간 후 파괴된 벽 정리
                    FTimerHandle DestroyTimerHandle;
                    FTimerDelegate DestroyDelegate;
                    DestroyDelegate.BindLambda([DestructibleWall, WallActor]()
                        {
                            if (DestructibleWall)
                            {
                                DestructibleWall->Destroy();
                            }
                        });

                    GetWorldTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, 10.0f, false);
                }
            }
        }
    }
}