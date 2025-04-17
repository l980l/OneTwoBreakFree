// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OTAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API UOTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AOTCharacterBase> OTCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	uint8 bIsSprinting : 1;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float ForwardMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RotationLastFrame;

	UPROPERTY(BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RotationCurrentFrame;

	UPROPERTY(BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float DeltaRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	uint8 bIsTurning : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	int32 TurnDirection; // -1 = 왼쪽, 1 = 오른쪽, 0 = 회전 없음
};
