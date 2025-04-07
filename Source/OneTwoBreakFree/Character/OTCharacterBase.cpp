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

AOTCharacterBase::AOTCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.0f);
	GetCapsuleComponent()->SetIsReplicated(true);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 1ÀÎÄª 
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

	// 3ÀÎÄª 
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
	ServerToggleSprint(true);
}

void AOTCharacterBase::SprintReleased(const FInputActionValue& Value)
{
	if (bIsSprinting)
	{
		ServerToggleSprint(false);
	}
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
