// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "OTDestructibleWall.generated.h"

/**
 * 
 */
UCLASS()
class ONETWOBREAKFREE_API AOTDestructibleWall : public AGeometryCollectionActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void SetDestructiveImpulse(float InImpulse, FVector InImpactPoint);

private:
	float Impulse;
	FVector ImpactPoint
};
