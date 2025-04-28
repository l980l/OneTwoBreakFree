// Fill out your copyright notice in the Description page of Project Settings.


#include "OTMatchGameState.h"
#include "PCGComponent.h"
#include "OneTwoBreakFree/Backrooms/OTBackroomsActor.h"
#include "OneTwoBreakFree/GameMode/OTMatchGameMode.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AOTMatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOTMatchGameState, PCGRandomSeed);
	DOREPLIFETIME(AOTMatchGameState, ResultsScreenDuration);
}

void AOTMatchGameState::OnRep_PCGRandomSeed()
{
	if (!HasAuthority() && PCGBackroomsActorClass)
	{
		UE_LOG(LogTemp, Log, TEXT("Client Received PCG Seed: %d"), PCGRandomSeed);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* BackroomsActor = GetWorld()->SpawnActor<AActor>(PCGBackroomsActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (BackroomsActor)
		{
			UPCGComponent* PCGComponent = BackroomsActor->FindComponentByClass<UPCGComponent>();
			if (PCGComponent)
			{
				PCGComponent->Seed = PCGRandomSeed;
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
}
