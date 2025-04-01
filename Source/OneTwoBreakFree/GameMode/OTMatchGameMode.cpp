// Fill out your copyright notice in the Description page of Project Settings.

#include "OTMatchGameMode.h"
#include "PCGComponent.h"
#include "OneTwoBreakFree/GameState/OTMatchGameState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AOTMatchGameMode::AOTMatchGameMode()
{
}

void AOTMatchGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		StartPCGMapGeneration();
	}
}

void AOTMatchGameMode::StartPCGMapGeneration()
{
	if (!HasAuthority() || !PCGBackroomsActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PCG Map Generation Fail"));
		return;
	}

	int32 PCGRandomSeed = FMath::RandRange(1, 999999);

	AOTMatchGameState* MatchGS = GetGameState<AOTMatchGameState>();
	if (MatchGS)
	{
		MatchGS->PCGRandomSeed = PCGRandomSeed;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PCGBackroomsActor = GetWorld()->SpawnActor<AActor>(PCGBackroomsActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (PCGBackroomsActor)
	{
		UPCGComponent* PCGComponent = PCGBackroomsActor->FindComponentByClass<UPCGComponent>();
		if (PCGComponent)
		{
			PCGComponent->Seed = PCGRandomSeed;
			PCGComponent->OnPCGGraphGeneratedExternal.AddDynamic(this, &AOTMatchGameMode::OnPCGGenerationCompleted);
			PCGComponent->GenerateLocal(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("can not find PCG Component"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PCG Backrooms Actor Spawning Fail"));
	}
}

void AOTMatchGameMode::OnPCGGenerationCompleted(UPCGComponent* InPCGComponent)
{
	if (!HasAuthority())
	{
		return;
	}

	bIsMapGenerated = true;

	TeleportPlayersToSpawnPoints();
	
	OnPCGMapGenerated.Broadcast();
}


TArray<FTransform> AOTMatchGameMode::FindPlayerSpawnPoints(int32 PlayerCount)
{
    TArray<FTransform> SpawnPoints;

    if (!PCGBackroomsActor)
    {
        return SpawnPoints;
    }

    TArray<AActor*> TaggedActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("PCG_SpawnPoint"), TaggedActors);

    for (AActor* SpawnActor : TaggedActors)
    {
        if (SpawnActor)
        {
            SpawnPoints.Add(SpawnActor->GetActorTransform());
        }
    }

    return SpawnPoints;
}

void AOTMatchGameMode::TeleportPlayersToSpawnPoints()
{
	if (!HasAuthority() || !bIsMapGenerated)
	{
		return;
	}

	TArray<APlayerController*> PlayerControllers;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		APlayerController* PC = iter->Get();
		if (PC)
		{
			PlayerControllers.Add(PC);
		}
	}

	TArray<FTransform> SpawnPoints = FindPlayerSpawnPoints(PlayerControllers.Num());

	for (int32 i = 0; (i < PlayerControllers.Num()) && (i < SpawnPoints.Num()); ++i)
	{
		APlayerController* PC = PlayerControllers[i];
		APawn* Pawn = PC->GetPawn();

		if (Pawn)
		{
			Pawn->SetActorTransform(SpawnPoints[i], false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
}
