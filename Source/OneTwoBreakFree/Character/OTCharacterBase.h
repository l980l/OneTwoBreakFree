// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "OTCharacterBase.generated.h"

UCLASS()
class ONETWOBREAKFREE_API AOTCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOTCharacterBase();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* ThirdPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    class USkeletalMeshComponent* FirstPersonMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", Replicated)
    float MaxStamina = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", Replicated)
    float Stamina = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    float StaminaConsumptionRate = 20.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    float StaminaRegenerationRate = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    float MinStaminaToSprint = 10.0f; // 달리기 위한 최소 스테미나

    // 이동 관련 속성
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float WalkSpeed = 400.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintSpeed = 800.0f;

    UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
    uint8 bIsSprinting : 1 = false;

    UFUNCTION()
    void OnRep_IsSprinting();

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* SprintAction;

    // Input 함수
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void SprintPressed(const FInputActionValue& Value);
    void SprintReleased(const FInputActionValue& Value);

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool CanSprint() const;

    void ConsumeStamina(float DeltaTime);

    void RegenerateStamina(float DeltaTime);

    // 달리기 함수
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerToggleSprint(bool bShouldSprint);

public:
    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetCurrentStamina() const;

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetMaxStamina() const;
};
