// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCitizenCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OneTwoBreakFree/CharacterComponent/OTHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "OTCharacterMovementComponent.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"
#include "OneTwoBreakFree/GameMode/OTMatchGameMode.h"
#include "OneTwoBreakFree/PlayerState/OTPlayerState.h"
#include "OneTwoBreakFree/Types/AnnouncementType.h"
#include "OneTwoBreakFree/Character/OTKillerCharacter.h"
#include "OneTwoBreakFree/Character/OTSpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

AOTCitizenCharacter::AOTCitizenCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	HealthComponent = CreateDefaultSubobject<UOTHealthComponent>(TEXT("HealthComponent"));
	CharacterRole = EOTCharacterRole::ECR_Citizen;
}

void AOTCitizenCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AOTCitizenCharacter::OnCharacterDeath);
		HealthComponent->OnHealthChanged.AddDynamic(this, &AOTCitizenCharacter::OnHealthChanged);
	}
}

void AOTCitizenCharacter::OnHealthChanged(UOTHealthComponent* HealthComp, float Health, float HealthDelta)
{
	if (IsLocallyControlled())
	{
		if (DamagedSound)
		{
			UGameplayStatics::PlaySound2D(this, DamagedSound);
		}

		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			PC->SetHUDHealth(Health / GetMaxHealth());
			PC->ClientStartCameraShake(FireCameraShake, 5.0f);
		}
	}
}

void AOTCitizenCharacter::OnCharacterDeath(UOTHealthComponent* HealthComp, AActor* KilledActor, AActor* KillerActor)
{
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	if (IsLocallyControlled())
	{
		if (DeadSound)
		{
			UGameplayStatics::PlaySound2D(this, DeadSound);
		}

		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			PC->SetHUDHealth(0.f);
			PC->ShowAnnouncement(EAnnouncementType::EANMT_Dead);
		}
	}

	if (HasAuthority())
	{
		SetLifeSpan(10.0f);

		AOTPlayerState* PS = GetPlayerState<AOTPlayerState>();
		if (PS)
		{
			PS->DeathTime = GetWorld()->GetTimeSeconds();
		}

		if (KillerActor)
		{
			AOTKillerCharacter* KillerCharacter = Cast<AOTKillerCharacter>(KillerActor);
			if (!KillerCharacter)
			{
				KillerCharacter = Cast<AOTKillerCharacter>(KillerActor->GetOwner());
			}

			if (KillerCharacter)
			{
				KillerCharacter->ClientPlayKillSound();

				AOTPlayerState* KillerPS = KillerCharacter->GetPlayerState<AOTPlayerState>();
				if (KillerPS && KillerPS->IsKiller())
				{
					KillerPS->KillCount++;
				}

				AOTPlayerController* KillerPC = Cast<AOTPlayerController>(KillerCharacter->GetController());
				if (KillerPC)
				{
					KillerPC->ClientShowAnnouncement(EAnnouncementType::EANMT_Kill);
				}
			}
		}

		AController* OldController = GetController();

		if (OldController)
		{
			OldController->UnPossess();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = OldController;

			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();

			AOTSpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<AOTSpectatorPawn>(
				SpectatorPawnClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

			if (SpectatorPawn)
			{
				OldController->Possess(SpectatorPawn);

				if (AOTPlayerController* PC = Cast<AOTPlayerController>(OldController))
				{
					PC->EnterSpectatorMode();
				}
			}
		}

		if (AOTMatchGameMode* GameMode = Cast<AOTMatchGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->CheckGameEndCondition();
		}
	}
}

void AOTCitizenCharacter::HandleEscape()
{
	if (IsLocallyControlled())
	{
		if (EscapeSound)
		{
			UGameplayStatics::PlaySound2D(this, EscapeSound);
		}

		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			PC->ShowAnnouncement(EAnnouncementType::EANMT_Survive);
		}
	}

	if (HasAuthority())
	{
		AOTPlayerState* PS = GetPlayerState<AOTPlayerState>();
		if (PS)
		{
			PS->EscapeTime = GetWorld()->GetTimeSeconds();
		}

		AController* OldController = GetController();

		if (OldController)
		{
			OldController->UnPossess();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = OldController;

			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();

			AOTSpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<AOTSpectatorPawn>(
				SpectatorPawnClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

			if (SpectatorPawn)
			{
				OldController->Possess(SpectatorPawn);

				if (AOTPlayerController* PC = Cast<AOTPlayerController>(OldController))
				{
					PC->EnterSpectatorMode();
				}
			}

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetLifeSpan(1.0f);
		}

		if (AOTMatchGameMode* GameMode = Cast<AOTMatchGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->CheckGameEndCondition();
		}
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
