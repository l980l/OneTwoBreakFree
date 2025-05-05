// Fill out your copyright notice in the Description page of Project Settings.

#include "OTSpectatorPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"
#include "OneTwoBreakFree/Character/OTCharacterBase.h"
#include "OneTwoBreakFree/Character/OTCitizenCharacter.h"
#include "OneTwoBreakFree/Character/OTKillerCharacter.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "GameFramework/PlayerState.h"

AOTSpectatorPawn::AOTSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);

	USpectatorPawnMovement* SpectatorMovement = Cast<USpectatorPawnMovement>(GetMovementComponent());

	if (SpectatorMovement)
	{
		SpectatorMovement->MaxSpeed = 0.f;
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
	Camera->bUsePawnControlRotation = false;
}

void AOTSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

	UpdateAvailableTargets();

	if (AvailableTargets.Num() > 0 && IsLocallyControlled())
	{
		ServerSwitchToTarget(AvailableTargets[0]);
	}
}

void AOTSpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bFreeCameraMode)
	{
		UpdateTargetTracking(DeltaTime);
	}

	static float TimeSinceLastUpdate = 0.0f;
	static const float UpdateInterval = 1.0f;

	TimeSinceLastUpdate += DeltaTime;
	if (TimeSinceLastUpdate >= UpdateInterval)
	{
		UpdateAvailableTargets();
		TimeSinceLastUpdate = 0.0f;

		// 현재 타겟이 없고 타겟 목록이 있으면 첫 번째 타겟으로 전환
		if (!bFreeCameraMode && !SpectatingTarget && AvailableTargets.Num() > 0 && IsLocallyControlled())
		{
			ServerSwitchToTarget(AvailableTargets[0]);
		}
	}
}

void AOTSpectatorPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOTSpectatorPawn, SpectatingTarget);
	DOREPLIFETIME(AOTSpectatorPawn, bFreeCameraMode);
}

void AOTSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		if (NextTargetAction)
		{
			EnhancedInputComponent->BindAction(NextTargetAction, ETriggerEvent::Started, this, &AOTSpectatorPawn::NextTarget);
		}

		if (PreviousTargetAction)
		{
			EnhancedInputComponent->BindAction(PreviousTargetAction, ETriggerEvent::Started, this, &AOTSpectatorPawn::PreviousTarget);
		}

		if (ToggleFreeCamAction)
		{
			EnhancedInputComponent->BindAction(ToggleFreeCamAction, ETriggerEvent::Started, this, &AOTSpectatorPawn::ToggleFreeCamera);
		}

		if (ToggleFreeCamAction)
		{
			EnhancedInputComponent->BindAction(OptionAction, ETriggerEvent::Started, this, &AOTSpectatorPawn::OptionPressed);
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			if (SpectatorMappingContext)
			{
				Subsystem->AddMappingContext(SpectatorMappingContext, 0);
			}
		}
	}
}

void AOTSpectatorPawn::NextTarget(const FInputActionValue& Value)
{
	if (bCanSwitchTarget && !bFreeCameraMode)
	{
		SwitchToNextTarget();

		bCanSwitchTarget = false;
		GetWorldTimerManager().SetTimer(SwitchTargetTimerHandle, this, &AOTSpectatorPawn::EnableTargetSwitch, TargetSwitchCooldown, false);
	}
}

void AOTSpectatorPawn::PreviousTarget(const FInputActionValue& Value)
{
	if (bCanSwitchTarget && !bFreeCameraMode)
	{
		SwitchToPreviousTarget();

		bCanSwitchTarget = false;
		GetWorldTimerManager().SetTimer(SwitchTargetTimerHandle, this, &AOTSpectatorPawn::EnableTargetSwitch, TargetSwitchCooldown, false);
	}
}

void AOTSpectatorPawn::ToggleFreeCamera(const FInputActionValue& Value)
{
	ToggleFreeCameraMode();
}

void AOTSpectatorPawn::OptionPressed(const FInputActionValue& Value)
{
	if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
	{
		PC->ToggleSettingUI();
	}
}

void AOTSpectatorPawn::EnableTargetSwitch()
{
	bCanSwitchTarget = true;
}

void AOTSpectatorPawn::UpdateAvailableTargets()
{
	AvailableTargets.Empty();

	TArray<AActor*> KillerCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOTKillerCharacter::StaticClass(), KillerCharacters);
	AvailableTargets.Append(KillerCharacters);

	TArray<AActor*> CitizenCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOTCitizenCharacter::StaticClass(), CitizenCharacters);

	for (AActor* Citizen : CitizenCharacters)
	{
		if (AOTCitizenCharacter* CitizenChar = Cast<AOTCitizenCharacter>(Citizen))
		{
			if (CitizenChar->IsAlive())
			{
				AvailableTargets.Add(Citizen);
			}
		}
	}

	if (!bFreeCameraMode && SpectatingTarget && !AvailableTargets.Contains(SpectatingTarget) && AvailableTargets.Num() > 0 && IsLocallyControlled())
	{
		ServerSwitchToTarget(AvailableTargets[0]);
	}
}

void AOTSpectatorPawn::SwitchToTarget(AActor* NewTarget)
{
	if (NewTarget && IsLocallyControlled())
	{
		ServerSwitchToTarget(NewTarget);
	}
}

void AOTSpectatorPawn::SwitchToNextTarget()
{
	if (AvailableTargets.Num() == 0)
	{
		return;
	}

	int32 CurrentIndex = AvailableTargets.Find(SpectatingTarget);
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = -1; // 현재 타겟이 목록에 없으면 첫 번째 타겟부터 시작
	}

	int32 NextIndex = (CurrentIndex + 1) % AvailableTargets.Num();

	ServerSwitchToTarget(AvailableTargets[NextIndex]);
}

void AOTSpectatorPawn::SwitchToPreviousTarget()
{
	if (AvailableTargets.Num() == 0)
	{
		return;
	}

	int32 CurrentIndex = AvailableTargets.Find(SpectatingTarget);
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = 0;
	}

	int32 PrevIndex = (CurrentIndex - 1 + AvailableTargets.Num()) % AvailableTargets.Num();

	ServerSwitchToTarget(AvailableTargets[PrevIndex]);
}

void AOTSpectatorPawn::UpdateTargetTracking(float DeltaTime)
{
	if (SpectatingTarget && IsLocallyControlled())
	{
		if (AOTCharacterBase* TargetCharacter = Cast<AOTCharacterBase>(SpectatingTarget))
		{
			FVector TargetLocation;
			FRotator TargetRotation;

			if (TargetCharacter->ThirdPersonCamera && TargetCharacter->ThirdPersonCamera->IsActive())
			{
				TargetLocation = TargetCharacter->ThirdPersonCamera->GetComponentLocation();
				TargetRotation = TargetCharacter->ThirdPersonCamera->GetComponentRotation();
			}

			FVector CurrentLocation = GetActorLocation();
			FRotator CurrentRotation = GetActorRotation();

			float PositionInterpolationSpeed = 10.0f;
			float RotationInterpolationSpeed = 10.0f;

			FVector NewLocation = FMath::VInterpTo(
				CurrentLocation,
				TargetLocation,
				DeltaTime,
				PositionInterpolationSpeed
			);

			FRotator NewRotation = FMath::RInterpTo(
				CurrentRotation,
				TargetRotation,
				DeltaTime,
				RotationInterpolationSpeed
			);

			SetActorLocationAndRotation(NewLocation, NewRotation);

			if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
			{
				if (TargetCharacter->GetPlayerState())
				{
					FString PlayerName = TargetCharacter->GetPlayerState()->GetPlayerName();
					PC->SetSpectatingPlayerInfo(PlayerName);
				}
			}
		}
	}
}

void AOTSpectatorPawn::ToggleFreeCameraMode()
{
	if (IsLocallyControlled())
	{
		if (bFreeCameraMode)
		{
			if (AvailableTargets.Num() > 0)
			{
				ServerToggleFreeCameraMode(false);
				ServerSwitchToTarget(AvailableTargets[0]);
			}
		}
		else
		{
			ServerToggleFreeCameraMode(true);
			ServerSwitchToTarget(nullptr);
		}
	}
}

void AOTSpectatorPawn::ServerSwitchToTarget_Implementation(AActor* NewTarget)
{
	SpectatingTarget = NewTarget;
	MulticastSwitchToTarget(NewTarget);
}

void AOTSpectatorPawn::MulticastSwitchToTarget_Implementation(AActor* NewTarget)
{
	SpectatingTarget = NewTarget;

	if (IsLocallyControlled())
	{
		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			if (SpectatingTarget)
			{
				if (AOTCharacterBase* Character = Cast<AOTCharacterBase>(SpectatingTarget))
				{
					if (Character->GetPlayerState())
					{
						FString PlayerName = Character->GetPlayerState()->GetPlayerName();
						PC->SetSpectatingPlayerInfo(PlayerName);
					}
				}
			}
			else
			{
				PC->SetSpectatingPlayerInfo("Free Mode");
			}
		}
	}
}

void AOTSpectatorPawn::ServerToggleFreeCameraMode_Implementation(bool bNewFreeCameraMode)
{
	bFreeCameraMode = bNewFreeCameraMode;
	MulticastToggleFreeCameraMode(bNewFreeCameraMode);
}

void AOTSpectatorPawn::MulticastToggleFreeCameraMode_Implementation(bool bNewFreeCameraMode)
{
	bFreeCameraMode = bNewFreeCameraMode;

	USpectatorPawnMovement* SpectatorMovement = Cast<USpectatorPawnMovement>(GetMovementComponent());

	if (SpectatorMovement)
	{
		if (bNewFreeCameraMode)
		{
			Camera->bUsePawnControlRotation = true;
			SpectatorMovement->MaxSpeed = 1000.f;
		}
		else
		{
			Camera->bUsePawnControlRotation = false;
			Camera->SetRelativeRotation(FRotator::ZeroRotator);
			SpectatorMovement->MaxSpeed = 0.f;
		}
	}
}