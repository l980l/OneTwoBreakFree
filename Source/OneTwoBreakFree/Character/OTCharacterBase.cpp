// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"
#include "OTCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"


AOTCharacterBase::AOTCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.0f);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;
	GetCharacterMovement()->bServerAcceptClientAuthoritativePosition = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 1인칭 
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetRootComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetRelativeLocation(FVector(-10.0f, 0.0f, -130.0f));
	FirstPersonMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// 3인칭 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 70.0f);

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	GetMesh()->SetupAttachment(GetRootComponent());
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastDynamicShadow = true;
	GetMesh()->CastShadow = true;
}

void AOTCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOTCharacterBase, MaxStamina);
	DOREPLIFETIME(AOTCharacterBase, Stamina);
	DOREPLIFETIME(AOTCharacterBase, bIsSprinting);
}

void AOTCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	Stamina = MaxStamina;
}

void AOTCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsLocallyControlled())
	{
		GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
	}
}

void AOTCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
}

void AOTCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (bIsSprinting)
		{
			ConsumeStamina(DeltaTime);

			if (Stamina <= 0.05f)
			{
				ServerToggleSprint(false);
			}
		}
		else
		{
			RegenerateStamina(DeltaTime);
		}
	}

	if (IsLocallyControlled() && bIsSprinting)
	{
		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			PC->SetHUDStamina(Stamina / MaxStamina);
		}
	}
}

void AOTCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOTCharacterBase::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOTCharacterBase::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOTCharacterBase::SprintPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOTCharacterBase::SprintReleased);
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started, this, &AOTCharacterBase::KickPressed);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AOTCharacterBase::Move(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AOTCharacterBase::Look(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AOTCharacterBase::SprintPressed(const FInputActionValue& Value)
{
	if (bIsKicking)
		return;

	ServerToggleSprint(true);
}

void AOTCharacterBase::SprintReleased(const FInputActionValue& Value)
{
	if (bIsSprinting)
	{
		ServerToggleSprint(false);
	}
}

void AOTCharacterBase::KickPressed(const FInputActionValue& Value)
{
	ServerKick();
}

void AOTCharacterBase::ConsumeStamina(float DeltaTime)
{
	if (HasAuthority())
	{
		Stamina = FMath::Max(0.0f, Stamina - StaminaConsumptionRate * DeltaTime);
	}
}

void AOTCharacterBase::RegenerateStamina(float DeltaTime)
{
	if (HasAuthority())
	{
		Stamina = FMath::Min(MaxStamina, Stamina + StaminaRegenerationRate * DeltaTime);
	}
}

void AOTCharacterBase::ServerToggleSprint_Implementation(bool bShouldSprint)
{
	if (bShouldSprint && (Stamina > MinStaminaToSprint))
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	if (IsLocallyControlled())
	{
		if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
		{
			PC->ShowHUDStamina(bIsSprinting);
		}
	}
}

void AOTCharacterBase::OnRep_IsSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;

	if (AOTPlayerController* PC = Cast<AOTPlayerController>(Controller))
	{
		PC->ShowHUDStamina(bIsSprinting);
	}
}

void AOTCharacterBase::ServerKick_Implementation()
{
	if (bIsKicking)
		return;

	MulticastKick();
}

void AOTCharacterBase::MulticastKick_Implementation()
{
	bIsKicking = true;

	if (ThirdPersonKickMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(ThirdPersonKickMontage, 1.f);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AOTCharacterBase::OnKickMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, ThirdPersonKickMontage);

		GetMesh()->SetOwnerNoSee(false);
		FirstPersonMesh->SetOwnerNoSee(true);
	}

	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_None;
}

void AOTCharacterBase::OnKickMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ThirdPersonKickMontage)
	{
		bIsKicking = false;
		GetMesh()->SetOwnerNoSee(true);
		FirstPersonMesh->SetOwnerNoSee(false);
	}

	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
}

void AOTCharacterBase::KickImpact()
{
	if (!IsLocallyControlled())
		return;

	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * KickRange;

	// 트레이스 실행
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
	{
		// 벽인지 확인
		AStaticMeshActor* WallActor = Cast<AStaticMeshActor>(HitResult.GetActor());
		if (WallActor && WallActor->Tags.Contains("DestructibleWall"))
		{
			if (!HasAuthority())
			{
				ServerTriggerWallDestruction(HitResult.ImpactPoint, WallActor->GetActorLocation(), WallActor->GetActorRotation());
			}
			else
			{
				MulticastTriggerWallDestruction(HitResult.ImpactPoint, WallActor->GetActorLocation(), WallActor->GetActorRotation());
			}
		}
	}
}

void AOTCharacterBase::ServerTriggerWallDestruction_Implementation(FVector_NetQuantize ImpactPoint, FVector_NetQuantize WallLocation, FRotator WallRotation)
{
	MulticastTriggerWallDestruction(ImpactPoint, WallLocation, WallRotation);
}

void AOTCharacterBase::MulticastTriggerWallDestruction_Implementation(FVector_NetQuantize ImpactPoint, FVector_NetQuantize WallLocation, FRotator WallRotation)
{
	TriggerWallDestruction(ImpactPoint, WallLocation, WallRotation);
}

void AOTCharacterBase::TriggerWallDestruction(FVector_NetQuantize ImpactPoint, FVector_NetQuantize WallLocation, FRotator WallRotation)
{
	// PCG로 생성된 위치에 있는 벽을 찾기
	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), WallLocation, 10.0f, TArray<TEnumAsByte<EObjectTypeQuery>>(), AStaticMeshActor::StaticClass(), TArray<AActor*>(), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AStaticMeshActor* WallActor = Cast<AStaticMeshActor>(Actor);
		if (WallActor && WallActor->Tags.Contains("DestructibleWall"))
		{
			// 기존 벽 비활성화
			WallActor->SetActorHiddenInGame(true);
			WallActor->SetActorEnableCollision(false);

			if (!DestructibleWallClass)
				break;

			// GeometryCollection 스폰
			FActorSpawnParameters SpawnParams;
			
			AGeometryCollectionActor* DestructibleWall = GetWorld()->SpawnActor<AGeometryCollectionActor>(DestructibleWallClass, WallLocation, WallRotation, SpawnParams);

			if (DestructibleWall)
			{
				// 스케일 맞추기
				DestructibleWall->SetActorScale3D(WallActor->GetActorScale3D());

				// 바로 파괴 트리거
				UGeometryCollectionComponent* GeoComp = DestructibleWall->GetGeometryCollectionComponent();
				if (GeoComp)
				{
					// 약간의 딜레이를 두고 파괴 (렌더링 보장)
					FTimerHandle BreakTimerHandle;

					GetWorld()->GetTimerManager().SetTimer(BreakTimerHandle, [GeoComp, ImpactPoint, WallLocation]()
						{
						FVector ImpactDirection = (ImpactPoint - WallLocation).GetSafeNormal();
						// 충격점에서 파괴력 적용
						GeoComp->AddImpulseAtLocation(ImpactDirection * 1000000.0f, ImpactPoint);

						}, 0.1f, false);

					// 제거 타이머 설정
					FTimerHandle DestroyTimerHandle;
					FTimerDelegate DestroyDelegate;
					DestroyDelegate.BindLambda([DestructibleWall, WallActor]()
						{
							if (DestructibleWall)
							{
								DestructibleWall->Destroy();
							}
							/*if (WallActor)
							{
								WallActor->Destroy();
							}*/
						});

					GetWorldTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, 10.0f, false);
				}
			}

			// 첫 번째 일치하는 벽만 처리
			break;
		}
	}
}
