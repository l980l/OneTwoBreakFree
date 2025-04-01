// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OTBackroomsActor.generated.h"

class USplineComponent;
class UPCGComponent;
class UPCGGraph;

UCLASS()
class ONETWOBREAKFREE_API AOTBackroomsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AOTBackroomsActor();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PCG")
	TObjectPtr<USplineComponent> PCGSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PCG")
	TObjectPtr<UPCGComponent> PCGComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="PCG")
	TObjectPtr<UPCGGraph> PCGGraph;
};
