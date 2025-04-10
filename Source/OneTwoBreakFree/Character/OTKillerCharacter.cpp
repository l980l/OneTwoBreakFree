// Fill out your copyright notice in the Description page of Project Settings.


#include "OTKillerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OTCharacterMovementComponent.h"

AOTKillerCharacter::AOTKillerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CharacterRole = EOTCharacterRole::ECR_Killer;
}
