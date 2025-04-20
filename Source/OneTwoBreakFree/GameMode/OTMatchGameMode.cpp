// Fill out your copyright notice in the Description page of Project Settings.

#include "OTMatchGameMode.h"
#include "PCGComponent.h"
#include "OneTwoBreakFree/GameState/OTMatchGameState.h"
#include "OneTwoBreakFree/PlayerController/OTPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "OneTwoBreakFree/GameInstance/OTGameInstance.h"
#include "GameFramework/PlayerState.h"

AOTMatchGameMode::AOTMatchGameMode()
{
    bDelayedStart = true;
}

void AOTMatchGameMode::BeginPlay()
{
	Super::BeginPlay();

    UOTGameInstance* GameInstance = Cast<UOTGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        MaxPlayers = GameInstance->PlayerCount;
    }

	StartPCGMapGeneration();
}

void AOTMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 CurrentPlayerCount = GetWorld()->GetNumPlayerControllers();

    UE_LOG(LogTemp, Log, TEXT("Player joined: %s. Current count: %d/%d"), *NewPlayer->GetName(), CurrentPlayerCount, MaxPlayers);

    CheckAndStartGameIfReady();
}

void AOTMatchGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();

    for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
    {
        AOTPlayerController* OTPlayer = Cast<AOTPlayerController>(*iter);
        if (OTPlayer)
        {
            OTPlayer->OnMatchStateSet(MatchState);
        }
    }
}

void AOTMatchGameMode::StartPCGMapGeneration()
{
	if (!PCGBackroomsActorClass)
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
	bIsMapGenerated = true;

	CalculateMapBounds();

    UE_LOG(LogTemp, Log, TEXT("PCG Map generation completed. Checking if game can start..."));

    CheckAndStartGameIfReady();
}

void AOTMatchGameMode::CheckAndStartGameIfReady()
{
    int32 CurrentPlayerCount = GetWorld()->GetNumPlayerControllers();

    if ((MatchState != MatchState::WaitingToStart) || !bIsMapGenerated || (CurrentPlayerCount < MaxPlayers))
        return;

    FTimerHandle StartGameTimerHandle;

    GetWorld()->GetTimerManager().SetTimer(
        StartGameTimerHandle,
        this,
        &AOTMatchGameMode::StartGame,
        5.f,
        false
    );
}

void AOTMatchGameMode::StartGame()
{
    if (MatchState == MatchState::InProgress)
        return;

    int32 CurrentPlayerCount = GetWorld()->GetNumPlayerControllers();
    UE_LOG(LogTemp, Log, TEXT("Starting game with %d players"), CurrentPlayerCount);

    AssignPlayerRoles();

    TArray<FVector> SpawnLocations = FindPlayerSpawnLocations(CurrentPlayerCount);

    TeleportPlayersToLocations(SpawnLocations);
    
    StartMatch();

    UE_LOG(LogTemp, Log, TEXT("Match started successfully!"));
}

void AOTMatchGameMode::CalculateMapBounds()
{
	if (!PCGBackroomsActor)
	{
		UE_LOG(LogTemp, Fatal, TEXT("PCGBackroomsActor is null! Cannot calculate map bounds."));
		return;
	}

	USplineComponent* SplineComp = PCGBackroomsActor->FindComponentByClass<USplineComponent>();

	if (!SplineComp || SplineComp->GetNumberOfSplinePoints() == 0)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Spline Component not found or has no points in PCGBackroomsActor!"));
		checkf(false, TEXT("Spline Component not found or has no points in PCGBackroomsActor!"));
		return;
	}

	FVector FirstPoint = SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	MapMin = MapMax = FirstPoint;

	// 모든 스플라인 포인트를 순회하며 경계 계산
	for (int32 i = 1; i < SplineComp->GetNumberOfSplinePoints(); ++i)
	{
		FVector Point = SplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);

		MapMin.X = FMath::Min(MapMin.X, Point.X);
		MapMin.Y = FMath::Min(MapMin.Y, Point.Y);
		MapMin.Z = FMath::Min(MapMin.Z, Point.Z);

		MapMax.X = FMath::Max(MapMax.X, Point.X);
		MapMax.Y = FMath::Max(MapMax.Y, Point.Y);
		MapMax.Z = FMath::Max(MapMax.Z, Point.Z);
	}

	// 여유 공간 추가
	float Margin = 1000.0f;
	MapMin -= FVector(Margin, Margin, 0);
	MapMax += FVector(Margin, Margin, 0);

	// Z 축 높이 설정
	MapMin.Z = FMath::Min(MapMin.Z, 0.0f);
	MapMax.Z = FMath::Max(MapMax.Z, 300.0f);

	UE_LOG(LogTemp, Log, TEXT("Map Bounds: Min(%f, %f, %f), Max(%f, %f, %f)"), MapMin.X, MapMin.Y, MapMin.Z, MapMax.X, MapMax.Y, MapMax.Z);
}

void AOTMatchGameMode::AssignPlayerRoles()
{
    TArray<APlayerController*> PlayerControllers;
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PC = Iterator->Get();
        if (PC)
        {
            PlayerControllers.Add(PC);
        }
    }

    if (PlayerControllers.Num() == 0)
        return;

    int32 KillerIndex = FMath::RandRange(0, PlayerControllers.Num() - 1);
    KillerPlayerController = PlayerControllers[KillerIndex];


    // 디버깅 코드
    for (int32 i = 0; i < PlayerControllers.Num(); i++)
    {
        APlayerController* PC = PlayerControllers[i];
        EOTCharacterRole CharacterRole = (i == KillerIndex) ? EOTCharacterRole::ECR_Killer : EOTCharacterRole::ECR_Citizen;

        UE_LOG(LogTemp, Log, TEXT("Assigned role to player %s: %s"), *PC->GetPlayerState<APlayerState>()->GetPlayerName(), (CharacterRole == EOTCharacterRole::ECR_Killer) ? TEXT("Killer") : TEXT("Citizen"));
    }
}

TArray<FVector> AOTMatchGameMode::FindPlayerSpawnLocations(int32 CountPlayers)
{
    TArray<FVector> SpawnLocations;
    UWorld* World = GetWorld();

    if (!World)
        return SpawnLocations;

    // 맵 크기 계산
    float MapSizeX = MapMax.X - MapMin.X;
    float MapSizeY = MapMax.Y - MapMin.Y;

    // 목표 섹터 수 설정 (플레이어 수에 따라 조정)
    int32 TargetSectorCount = FMath::Max(4, CountPlayers * 2);

    // 맵 면적의 제곱근에 기반한 섹터 크기 계산
    float MapArea = MapSizeX * MapSizeY;
    float SectorSize = FMath::Sqrt(MapArea / TargetSectorCount);

    // 최소/최대 섹터 크기 제한
    SectorSize = FMath::Clamp(SectorSize, 1000.0f, 5000.0f);

    UE_LOG(LogTemp, Log, TEXT("Map size: %f x %f, Using sector size: %f"), MapSizeX, MapSizeY, SectorSize);

    // 계산된 섹터 크기로 맵 분할
    const int32 NumSectorsX = FMath::CeilToInt(MapSizeX / SectorSize);
    const int32 NumSectorsY = FMath::CeilToInt(MapSizeY / SectorSize);

    // 각 섹터에서 플레이어 하나씩 배치 시도 (가능한 경우)
    int32 PlayersRemaining = CountPlayers;
    int32 AttemptsPerSector = FMath::Max(5, MaxSpawnAttempts / (NumSectorsX * NumSectorsY));

    for (int32 SectorX = 0; SectorX < NumSectorsX && PlayersRemaining > 0; SectorX++)
    {
        for (int32 SectorY = 0; SectorY < NumSectorsY && PlayersRemaining > 0; SectorY++)
        {
            // 섹터 경계 계산
            float MinX = MapMin.X + SectorX * SectorSize;
            float MinY = MapMin.Y + SectorY * SectorSize;
            float MaxX = FMath::Min(MinX + SectorSize, MapMax.X);
            float MaxY = FMath::Min(MinY + SectorSize, MapMax.Y);

            // 이 섹터에서 위치 시도
            for (int32 Attempt = 0; Attempt < AttemptsPerSector; Attempt++)
            {
                // 섹터 내 랜덤 X,Y 좌표
                float RandomX = FMath::FRandRange(MinX, MaxX);
                float RandomY = FMath::FRandRange(MinY, MaxY);

                // 위에서 아래로 레이캐스트하여 바닥 찾기
                FVector Start(RandomX, RandomY, MapMax.Z);
                FVector End(RandomX, RandomY, MapMin.Z);

                FHitResult HitResult;
                FCollisionQueryParams QueryParams;

                bool bHit = World->LineTraceSingleByChannel(
                    HitResult,
                    Start,
                    End,
                    ECC_Visibility,
                    QueryParams
                );

                // 바닥을 찾았고, 위치가 유효한지 확인
                if (bHit)
                {
                    // 바닥 위에 적절한 높이로 위치 설정
                    FVector PotentialLocation = HitResult.Location + FVector(0, 0, PlayerSpawnHeight);

                    if (IsValidSpawnLocation(PotentialLocation, SpawnLocations))
                    {
                        SpawnLocations.Add(PotentialLocation);
                        PlayersRemaining--;
                        UE_LOG(LogTemp, Log, TEXT("Found spawn location at: (%f, %f, %f)"), PotentialLocation.X, PotentialLocation.Y, PotentialLocation.Z);
                        break;
                    }
                }
            }
        }
    }

    if (SpawnLocations.Num() < CountPlayers)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not find enough spawn locations. Found: %d, Needed: %d"), SpawnLocations.Num(), CountPlayers);
    }

    return SpawnLocations;
}

bool AOTMatchGameMode::IsValidSpawnLocation(const FVector& Location, const TArray<FVector>& ExistingLocations)
{
    UWorld* World = GetWorld();
    if (!World)
        return false;

    for (const FVector& ExistingLocation : ExistingLocations)
    {
        float Distance = FVector::Dist2D(Location, ExistingLocation);
        if (Distance < MinDistanceBetweenPlayers)
        {
            return false;
        }
    }

    FCollisionShape SphereShape = FCollisionShape::MakeSphere(MinDistanceFromWalls);
    FCollisionQueryParams QueryParams;

    bool bHasHit = World->OverlapAnyTestByChannel(
        Location,
        FQuat::Identity,
        ECC_WorldStatic,
        SphereShape,
        QueryParams
    );

    return !bHasHit;
}

void AOTMatchGameMode::TeleportPlayersToLocations(const TArray<FVector>& Locations)
{
    if (Locations.Num() == 0)
        return;

    TArray<APlayerController*> PlayerControllers;
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PC = Iterator->Get();
        if (PC)
        {
            PlayerControllers.Add(PC);
        }
    }

    int32 LocationIndex = 0;
    for (APlayerController* PC : PlayerControllers)
    {
        if (PC && Locations.Num() > 0)
        {
            int32 SafeIndex = LocationIndex % Locations.Num();
            FVector SpawnLocation = Locations[SafeIndex];

            if (PC->GetPawn())
            {
                PC->GetPawn()->Destroy();
            }

            TSubclassOf<APawn> OriginalDefaultPawnClass = DefaultPawnClass;

            ensureMsgf(KillerCharacterClass && CitizenCharacterClass, TEXT("KillerCharacterClass or CitizenCharacterClass has not been set."));

            if (PC == KillerPlayerController)
            {
                DefaultPawnClass = KillerCharacterClass;
            }
            else
            {
                DefaultPawnClass = CitizenCharacterClass;
            }

            // RestartPlayerAtTransform(PC, FTransform(SpawnLocation));
            RestartPlayerAtTransform(PC, FTransform(FVector(-100, 100, 120)));

            DefaultPawnClass = OriginalDefaultPawnClass;

            UE_LOG(LogTemp, Log, TEXT("Teleported player %s to: (%f, %f, %f)"), *PC->GetName(), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);

            LocationIndex++;
        }
    }
}
