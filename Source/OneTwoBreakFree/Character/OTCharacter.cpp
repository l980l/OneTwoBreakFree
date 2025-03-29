// Fill out your copyright notice in the Description page of Project Settings.


#include "OTCharacter.h"

// Sets default values
AOTCharacter::AOTCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOTCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AOTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

