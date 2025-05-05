// Fill out your copyright notice in the Description page of Project Settings.


#include "OTRifle.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"

AOTRifle::AOTRifle()
{
    MuzzleSocketName = "MuzzleFlash";
    MagazineCapacity = 8;
    CurrentAmmo = MagazineCapacity;
    FireRate = 0.2f;  // 초당 5발
    ReloadTime = 1.5f;
    LastFireTime = 0.f;
    Damage = 25.0f;
    AttackRange = 10000.0f;
    bCanFire = true;
    bIsReloading = false;
}

void AOTRifle::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = MagazineCapacity;
    bCanFire = true;
    bIsReloading = false;
}

void AOTRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOTRifle, CurrentAmmo);
    DOREPLIFETIME(AOTRifle, bCanFire);
    DOREPLIFETIME(AOTRifle, bIsReloading);
}

void AOTRifle::Fire()
{
    // 서버나 소유한 클라이언트만 처리
    if (GetLocalRole() != ROLE_Authority && !Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        return;
    }

    if (bCanFire && !bIsReloading && CurrentAmmo > 0)
    {
        if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
        {
            PlayFireEffects();
        }

        if (GetLocalRole() == ROLE_Authority)
        {
            OnWeaponFire.Broadcast();
            MulticastFireEffects();

            FireLineTrace();

            --CurrentAmmo;

            SetAmmoWidget();

            if (CurrentAmmo <= 0)
            {
                Reload();
            }

            bCanFire = false;
            LastFireTime = GetWorld()->GetTimeSeconds();

            GetWorldTimerManager().SetTimer(
                FireTimerHandle,
                [this]() { bCanFire = true; },
                FireRate,
                false
            );
        }

        // 클라에서도 재장전 애니메이션 및 소리가 나오도록 처리.
        else if (CurrentAmmo <= 1)
        {
            PlaySound(EWeaponSoundType::Reload);
        }
    }
}

void AOTRifle::Reload()
{
    if (GetLocalRole() != ROLE_Authority && !Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        return;
    }

    if (bIsReloading || CurrentAmmo >= MagazineCapacity)
    {
        return;
    }

    PlaySound(EWeaponSoundType::Reload);

    if (GetLocalRole() == ROLE_Authority)
    {
        OnWeaponReload.Broadcast();
        bIsReloading = true;

        GetWorld()->GetTimerManager().SetTimer(
            ReloadTimerHandle,
            [this]()
            {
                CurrentAmmo = MagazineCapacity;
                bIsReloading = false;
                SetAmmoWidget();
            },
            ReloadTime,
            false
        );
    }
}

void AOTRifle::MulticastFireEffects_Implementation()
{
    if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        return;
    }

    PlayFireEffects();
}

void AOTRifle::MulticastImpactEffects_Implementation(FVector_NetQuantize ImpactPoint, FVector_NetQuantize ImpactNormal)
{
    if (ImpactEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, ImpactPoint, ImpactNormal.Rotation());
    }
}

void AOTRifle::ClientFlashHitMarker_Implementation()
{
    if (OwnerPlayerController)
    {
        AOTPlayerController* PC = Cast<AOTPlayerController>(OwnerPlayerController);
        if (PC)
        {
            PC->FlashCrosshairRed();
            PC->FlashHitMarker();
        }
    }
}

void AOTRifle::FireLineTrace()
{
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }

    FVector EyeLocation;
    FRotator EyeRotation;
    OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    FVector ShotDirection = EyeRotation.Vector();

    FVector TraceEnd = EyeLocation + (ShotDirection * AttackRange);

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = true;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        EyeLocation,
        TraceEnd,
        ECC_Pawn,
        QueryParams
    );

    if (bHit)
    {
        AActor* HitActor = Hit.GetActor();

        // 데미지 적용
        if (HitActor && HitActor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyPointDamage(
                HitActor,                       
                Damage,                         
                ShotDirection,                  
                Hit,                           
                OwnerCharacter->GetController(),
                this,                          
                nullptr                        
            );
        
            ClientFlashHitMarker();
        }

        MulticastImpactEffects(Hit.ImpactPoint, Hit.ImpactNormal);
    }
}

void AOTRifle::PlayFireEffects()
{
    if (MuzzleFlash)
    {
        USkeletalMeshComponent* WeaponSkeletalMesh = Cast<USkeletalMeshComponent>(WeaponMesh);
        if (WeaponSkeletalMesh && !Cast<ACharacter>(GetOwner())->IsLocallyControlled())
        {
            const FTransform MuzzleTransform = WeaponSkeletalMesh->GetSocketTransform(MuzzleSocketName);
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
        }
        if (FirstPersonWeaponMesh && Cast<ACharacter>(GetOwner())->IsLocallyControlled())
        {
            const FTransform MuzzleTransform = FirstPersonWeaponMesh->GetSocketTransform(MuzzleSocketName);
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
        }
    }

    if (FireSound)
    {
        PlaySound(EWeaponSoundType::Fire);
    }

    APawn* OwningPawn = Cast<APawn>(GetOwner());
    if (OwningPawn && OwningPawn->IsLocallyControlled())
    {
        APlayerController* PC = Cast<APlayerController>(OwningPawn->GetController());
        if (PC)
        {
            PC->ClientStartCameraShake(FireCameraShake, 5.f);
        }
    }
}

void AOTRifle::SetAmmoWidget()
{
    if (OwnerPlayerController)
    {
        OwnerPlayerController->SetHUDRifleAmmo(CurrentAmmo, MagazineCapacity);
    }
}

void AOTRifle::OnRep_CurrentAmmo()
{
    SetAmmoWidget();
}
