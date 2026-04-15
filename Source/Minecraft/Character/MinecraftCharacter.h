// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnhancedInputSubsystems.h"
#include "CoreMinimal.h"
#include "Minecraft/Input/MinecraftInputComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "MinecraftCharacter.generated.h"

DECLARE_DELEGATE_TwoParams(FPlayerChunkPositionDelegate, const FIntPoint &, const FIntPoint &);
DECLARE_DELEGATE_TwoParams(FPlayerBlockPositionDelegate, const FIntVector &, const FIntVector &);
DECLARE_DELEGATE_TwoParams(FPlayerWorldPositionDelegate, const FVector &, const FVector &);
DECLARE_DELEGATE_OneParam(FPlayerMineDelegate, const FVector &);
DECLARE_DELEGATE_TwoParams(FPlayerPlaceDelegate, const FVector &, EBlockType Type);


UENUM()
enum class ECharacterState
{
	NONE,
	WALKING,
	WATERWALKING,
	SWIMMING,
};

UCLASS()
class MINECRAFT_API AMinecraftCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMinecraftCharacter();

	ECharacterState GetState() const;

	/*
	 * Start crouching, works only when player is on land or at the first block in water
	 */
	void StartCrouch();

	/*
	 * End crouching
	 */
	void EndCrouch();

	/*
	 * Start swimming, activates only when player is in the water
	 */
	void StartSwimming();

	/*
	 * Set walking in water. Works only when the water level is below the chest level
	 */
	void StartWalkingInWater();

	/*
	 * Start walking. Works only on land
	 */
	void StartWalking();

	/*
	 * Starts Cheat mode (Flying)
	 */
	void StartCheatMode();

	/*
	 * Ends Cheat mode and switch to the correct mode
	 */
	void EndCheatMode();

	/*
	 * Returns world position of the eye level
	 */
	FVector GetEyePosition() const;

	/*
	 * Returns world position of the chest level
	 */
	FVector GetChestPosition() const;

	/*
	 * Returns world position of the feet level
	 */
	FVector GetFeetPosition() const;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/*
	 * Shows menu
	 */
	void ShowMenu();

	/*
	 * Update players position and notifies everyone interested through delegates
	 */
	void UpdatePosition();

	/*
	 * Handles moving input
	 */
	void InputMove(const FInputActionValue& InputActionValue);

	/*
	 * Handles look input (mouse)
	 */
	void InputLook(const FInputActionValue& InputActionValue);

	/*
	 * Handles left hand action (mouse left click)
	 */
	void InputLeftHand(const FInputActionValue& InputActionValue);

	/*
	 * Handles left hand action (mouse right click)
	 */
	void InputRightHand(const FInputActionValue& InputActionValue);

	/*
	 * Handles fly up/down input
	 */
	void InputFly(const FInputActionValue& InputActionValue);

	/*
	 * Handles swim up input
	 */
	void InputSwim(const FInputActionValue& InputActionValue);

	/*
	 * Handles the cheat mode input
	 */
	void InputCheatMode(const FInputActionValue& InputActionValue);

	/*
	 * Returns the Enhanced input local player subsystem
	 */
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputLocalPlayerSubsystem();

	ULocalPlayer* GetLocalPlayer();

public:
	FPlayerBlockPositionDelegate OnBlockPositionChange;
	FPlayerChunkPositionDelegate OnChunkPositionChange;
	FPlayerWorldPositionDelegate OnWorldPositionChange;
	FPlayerMineDelegate OnMine;
	FPlayerPlaceDelegate OnPlace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UPlayerInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> BaseInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> WalkingInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> FlyingInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> SwimmingInputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minecraft")
	float FlyingFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minecraft")
	float SwimmingFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minecraft")
	float WalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minecraft")
	float WaterWalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minecraft")
	float RunningSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minecraft")
	float ReachDistance;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Feet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Chest;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> ActiveInputMapping;

	FIntVector BlockPosition;
	FVector WorldPosition;
	FIntPoint ChunkPosition;

	bool bPaused;
	bool bCheatMode;

	ECharacterState State;
};
