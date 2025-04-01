// Fill out your copyright notice in the Description page of Project Settings.


#include "OTBackroomsActor.h"
#include "PCGComponent.h"
#include "PCGGraph.h"
#include "Components/SplineComponent.h"

AOTBackroomsActor::AOTBackroomsActor()
{
	PrimaryActorTick.bCanEverTick = false;
	/*bReplicates = true;
	bAlwaysRelevant = true;*/

	PCGSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = PCGSpline;

	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCG"));

	PCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand;
	PCGComponent->bActivated = true;
}

void AOTBackroomsActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (PCGGraph)
	{
		PCGComponent->SetGraph(PCGGraph);
	}
}
