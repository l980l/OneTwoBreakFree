// Fill out your copyright notice in the Description page of Project Settings.


#include "OTAnimInstance.h"
#include "OTCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

void UOTAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OTCharacter = Cast<AOTCharacterBase>(TryGetPawnOwner());
}

void UOTAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!OTCharacter)
	{
		if ((OTCharacter = Cast<AOTCharacterBase>(TryGetPawnOwner())) == nullptr)
			return;
	}

	FVector Velocity = OTCharacter->GetVelocity();
	Speed = Velocity.Size();
	bIsSprinting = OTCharacter->GetIsSprinting();

	if (Speed > 0.0f)
	{
		const FRotator ActorRotation = OTCharacter->GetActorRotation();

		const FRotator VelocityRotation = UKismetMathLibrary::MakeRotFromX(Velocity);

		float YawDelta = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRotation, ActorRotation).Yaw;
		Direction = YawDelta;

		const FVector ForwardVector = OTCharacter->GetActorForwardVector();
		ForwardMovement = FVector::DotProduct(ForwardVector, Velocity.GetSafeNormal()) * 100.0f;

		ForwardMovement = FMath::Clamp(ForwardMovement, -100.0f, 100.0f);
	}
	else
	{
		Direction = 0.0f;
		ForwardMovement = 0.0f;
	}
}
