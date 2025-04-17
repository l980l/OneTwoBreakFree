 // Fill out your copyright notice in the Description page of Project Settings.


#include "OTKillerAnimInstance.h"
#include "OTCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

void UOTKillerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UOTKillerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!OTCharacter)
	{
		if ((OTCharacter = Cast<AOTCharacterBase>(TryGetPawnOwner())) == nullptr)
			return;
	}

	if (OTCharacter && OTCharacter->IsLocallyControlled() == false)
	{
		FRotator AimRotation = OTCharacter->GetBaseAimRotation();
		FRotator ActorRotation = OTCharacter->GetActorRotation();
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, ActorRotation);
		Yaw = DeltaRot.Yaw;
		Pitch = DeltaRot.Pitch;
	}
}
