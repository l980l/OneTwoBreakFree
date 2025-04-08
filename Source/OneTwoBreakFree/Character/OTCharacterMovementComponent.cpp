// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCharacterMovementComponent.h"

void UOTCharacterMovementComponent::ClientAdjustPosition(float TimeStamp, FVector NewLoc, FVector NewVel, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	Super::ClientAdjustPosition(TimeStamp, NewLoc, NewVel, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
}

void UOTCharacterMovementComponent::ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode, TOptional<FRotator> OptionalRotation)
{    
    // 위치 조정 전 로깅
    FVector OldLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
    UE_LOG(LogTemp, Warning, TEXT("Before Adjustment - Old Loc: %s, New Loc: %s"), *OldLocation.ToString(), *NewLoc.ToString());

    Super::ClientAdjustPosition_Implementation(TimeStamp, NewLoc, NewVel, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);

    // 위치 조정 후 로깅
    FVector NewLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
    UE_LOG(LogTemp, Warning, TEXT("After Adjustment - Location: %s, Target was: %s, Changed: %s"), *NewLocation.ToString(), *NewLoc.ToString(), (NewLocation.Equals(NewLoc, 0.1f) ? TEXT("YES") : TEXT("NO")));

    // 추가 디버깅을 위한 정보들
    UE_LOG(LogTemp, Warning, TEXT("MovementMode: %d, ServerMovementMode: %d"), MovementMode, ServerMovementMode);
}
