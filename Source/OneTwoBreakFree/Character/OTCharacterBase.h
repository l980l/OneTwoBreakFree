// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "OTCharacterBase.generated.h"

UENUM(BlueprintType)
enum class EOTCharacterRole : uint8
{
    ECR_None        UMETA(DisplayName = "None"),
    ECR_Killer      UMETA(DisplayName = "Killer"),
    ECR_Citizen     UMETA(DisplayName = "Citizen"),
    ECR_Spectator   UMETA(DisplayName = "Spectator")
};

UCLASS()
class ONETWOBREAKFREE_API AOTCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOTCharacterBase(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Character")
    EOTCharacterRole CharacterRole = EOTCharacterRole::ECR_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<class UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<class UCameraComponent> ThirdPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<class USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    TObjectPtr<class USkeletalMeshComponent> FirstPersonMesh;

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_Controller() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    void KickImpact();

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
    float MaxStamina = 100.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stamina")
    float Stamina = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    float StaminaConsumptionRate = 20.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    float StaminaRegenerationRate = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    float MinStaminaToSprint = 10.0f; // 달리기 위한 최소 스테미나

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float WalkSpeed = 200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintSpeed = 400.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kick")
    float KickRange = 150.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputAction> SprintAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<class UInputAction> KickAction;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<class UAnimMontage> ThirdPersonKickMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<class UAnimMontage> FirstPersonKickMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kick")
    TSubclassOf<class AGeometryCollectionActor> DestructibleWallClass;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<class UCameraShakeBase> FireCameraShake;

    UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
    uint8 bIsSprinting : 1 = false;

    uint8 bIsKicking : 1 = false;

private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void SprintPressed(const FInputActionValue& Value);
    void SprintReleased(const FInputActionValue& Value);
    void KickPressed(const FInputActionValue& Value);

    void ConsumeStamina(float DeltaTime);
    void RegenerateStamina(float DeltaTime);

    void SetupThirdPersonMesh();

    UFUNCTION(Server, Reliable)
    void ServerToggleSprint(bool bShouldSprint);

    UFUNCTION(Server, Reliable)
    void ServerKick();
    
    UFUNCTION(NetMulticast, Reliable)
    void MulticastKick();

    UFUNCTION(Server, Reliable)
    void ServerTriggerWallDestruction(FVector_NetQuantize ImpactPoint, FVector_NetQuantize WallLocation, FRotator WallRotation);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastTriggerWallDestruction(FVector_NetQuantize ImpactPoint, FVector_NetQuantize WallLocation, FRotator WallRotation);

    void OnKickMontageEnded(class UAnimMontage* Montage, bool bInterrupted);

    void TriggerWallDestruction(FVector_NetQuantize ImpactPoint, FVector_NetQuantize WallLocation, FRotator WallRotation);

    UFUNCTION()
    void OnRep_IsSprinting();

public:
    FORCEINLINE EOTCharacterRole GetCharacterRole() const { return CharacterRole; }
    FORCEINLINE float GetStamina() const { return Stamina; }
    FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
    FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
    FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
    FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
};
