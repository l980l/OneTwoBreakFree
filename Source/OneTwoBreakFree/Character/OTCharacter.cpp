// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCharacter.h"

AOTCharacter::AOTCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AOTCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

