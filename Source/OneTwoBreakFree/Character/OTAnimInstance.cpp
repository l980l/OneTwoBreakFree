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

		if (YawDelta > 90.f)
		{
			YawDelta = 180.f - YawDelta;
		}
		else if (YawDelta < -90.f)
		{
			YawDelta = -180.f - YawDelta;
		}

		const float ProperSizeUpNum = 2.f;
		Direction = YawDelta * ProperSizeUpNum;
		Direction = FMath::Clamp(Direction, -100.0f, 100.0f);

		const FVector ForwardVector = OTCharacter->GetActorForwardVector();
		ForwardMovement = FVector::DotProduct(ForwardVector, Velocity.GetSafeNormal()) * 100.0f * ProperSizeUpNum;

		ForwardMovement = FMath::Clamp(ForwardMovement, -100.0f, 100.0f);
	}
	else
	{
		Direction = 0.0f;
		ForwardMovement = 0.0f;
	}
}
