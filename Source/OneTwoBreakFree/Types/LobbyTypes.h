#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyTypes.generated.h"

USTRUCT(BlueprintType)
struct FLobbyPlayerInfo
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<APlayerController> PlayerController = nullptr;

    UPROPERTY(BlueprintReadOnly)
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly)
    bool bIsReadyForMatch = false;
};
