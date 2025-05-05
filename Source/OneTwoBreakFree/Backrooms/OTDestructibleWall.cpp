// Fill out your copyright notice in the Description page of Project Settings.


#include "OTDestructibleWall.h"

void AOTDestructibleWall::BeginPlay()
{
	
}

void AOTDestructibleWall::SetDestructiveImpulse(float InImpulse, FVector InImpactPoint)
{
	Impulse = InImpulse;
	ImpactPoint = InImpactPoint;
}
