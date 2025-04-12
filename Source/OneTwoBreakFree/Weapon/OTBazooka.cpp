// Fill out your copyright notice in the Description page of Project Settings.


#include "OTBazooka.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "OTRocket.h"

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
        // 빈 발사 소리 재생
        return;
    }

    if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
    {
        PlayFireEffects();
    }
    MulticastFireEffects();

    // 서버에서 실제 발사 처리
    if (GetLocalRole() == ROLE_Authority)
    {
        --CurrentAmmo;

        ServerFireProjectile();

        bCanFire = false;
        bIsOnCooldown = true;
        CooldownRemaining = Cooldown;

        if (CurrentAmmo <= 0)
        {
            // 쿨다운 타이머 시작 (쿨다운 완료 후 1발 충전)
            GetWorld()->GetTimerManager().SetTimer(
                CooldownTimerHandle,
                this,
                &AOTBazooka::OnCooldownComplete,
                Cooldown,
                false
            );
        }
        else
        {
            // 발사 딜레이 후 다시 발사 가능하게 설정
            FTimerHandle FireDelayHandle;
            GetWorld()->GetTimerManager().SetTimer(
                FireDelayHandle,
                [this]() { bCanFire = true; },
                0.5f,  // 발사 딜레이
                false
            );
        }
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

void AOTBazooka::OnRep_CooldownRemaining()
{
    // 쿨다운 UI 업데이트
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
        if (FirstPersonWeaponMesh)
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
            PC->ClientStartCameraShake(FireCameraShake, 2.f);
        }
    }
}
