// Fill out your copyright notice in the Description page of Project Settings.


#include "OTBazooka.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OTRocket.h"
#include "GameFramework/Character.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"

AOTBazooka::AOTBazooka()
{
    PrimaryActorTick.bCanEverTick = true;

    MuzzleSocketName = "MuzzleFlash";
    ProjectileSpeed = 2000.0f;
    Cooldown = 10.0f;
    MaxAmmo = 1;
    CurrentAmmo = MaxAmmo;
    CooldownRemaining = 0.0f;
    bCanFire = true;
    bIsOnCooldown = false;
}

void AOTBazooka::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = MaxAmmo;
    bCanFire = true;
    bIsOnCooldown = false;
}

void AOTBazooka::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 서버에서만 쿨다운 업데이트
    if (GetLocalRole() == ROLE_Authority && bIsOnCooldown)
    {
        CooldownRemaining = FMath::Max(0.0f, CooldownRemaining - DeltaTime);

        if (CooldownRemaining <= 0.0f && CurrentAmmo < MaxAmmo)
        {
            OnCooldownComplete();
        }
    }

    if (OwnerPlayerController)
    {
        if (!bIsOnCooldown)
        {
            OwnerPlayerController->SetHUDBazookaPercent(1.f);
        }
        else
        {
            OwnerPlayerController->SetHUDBazookaPercent((Cooldown - CooldownRemaining) / Cooldown);
        }
    }
}

void AOTBazooka::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOTBazooka, CurrentAmmo);
    DOREPLIFETIME(AOTBazooka, CooldownRemaining);
    DOREPLIFETIME(AOTBazooka, bCanFire);
    DOREPLIFETIME(AOTBazooka, bIsOnCooldown);
}

void AOTBazooka::Fire()
{
    if (!bCanFire || CurrentAmmo <= 0)
    {
        return;
    }

    if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        PlayFireEffects();
    }
    MulticastFireEffects();

    if (GetLocalRole() == ROLE_Authority)
    {
        OnWeaponFire.Broadcast();
        --CurrentAmmo;

        ServerFireProjectile();

        bCanFire = false;
        bIsOnCooldown = true;
        CooldownRemaining = Cooldown;
    }
}

void AOTBazooka::ServerFireProjectile_Implementation()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }

    USkeletalMeshComponent* WeaponMeshComp = Cast<USkeletalMeshComponent>(WeaponMesh);
    if (!WeaponMeshComp)
    {
        return;
    }

    const FVector MuzzleLocation = WeaponMeshComp->GetSocketLocation(MuzzleSocketName);

    FVector EyeLocation;
    FRotator EyeRotation;
    OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    const FVector ShotDirection = EyeRotation.Vector();

    if (RocketClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = OwnerCharacter;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AOTRocket* Rocket = GetWorld()->SpawnActor<AOTRocket>(
            RocketClass,
            MuzzleLocation,
            EyeRotation,
            SpawnParams
        );

        if (Rocket)
        {
            Rocket->InitVelocity(ShotDirection);
            Rocket->SetOwner(GetOwner());
        }
    }
}

void AOTBazooka::MulticastFireEffects_Implementation()
{
    if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        return;
    }

    PlayFireEffects();
}

void AOTBazooka::OnCooldownComplete()
{
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    CooldownRemaining = 0.0f;
    bIsOnCooldown = false;

    CurrentAmmo = FMath::Min(MaxAmmo, CurrentAmmo + 1);

    bCanFire = true;
}

void AOTBazooka::PlayFireEffects()
{
    PlaySound(EWeaponSoundType::Fire);

    if (MuzzleFlash)
    {
        USkeletalMeshComponent* WeaponSkeletalMesh = Cast<USkeletalMeshComponent>(WeaponMesh);
        if (WeaponSkeletalMesh)
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

    APawn* OwningPawn = Cast<APawn>(GetOwner());
    if (OwningPawn && OwningPawn->IsLocallyControlled())
    {
        APlayerController* PC = Cast<APlayerController>(OwningPawn->GetController());
        if (PC)
        {
            PC->ClientStartCameraShake(FireCameraShake, 10.f);
        }
    }
}
