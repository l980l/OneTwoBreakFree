#pragma once

UENUM(BlueprintType)
enum class EOTCharacterRole : uint8
{
    ECR_None        UMETA(DisplayName = "None"),
    ECR_Killer      UMETA(DisplayName = "Killer"),
    ECR_Citizen     UMETA(DisplayName = "Citizen"),
    ECR_Spectator   UMETA(DisplayName = "Spectator")
};

