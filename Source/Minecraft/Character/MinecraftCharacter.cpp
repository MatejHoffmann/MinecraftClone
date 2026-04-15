// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "InventoryComponent.h"
#include "Minecraft/MinecraftGameplayTags.h"
#include "Minecraft/Input/MinecraftInputComponent.h"
#include "VectorTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "GenericPlatform/GenericPlatformSurvey.h"
#include "Kismet/KismetMathLibrary.h"
#include "Minecraft/UI/MinecraftHUD.h"
#include "Minecraft/World/Chunk.h"

// Sets default values
AMinecraftCharacter::AMinecraftCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, BaseEyeHeight)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Feet = CreateDefaultSubobject<UBoxComponent>(TEXT("FeetCollision"));
	Feet->SetupAttachment(GetCapsuleComponent());

	Chest = CreateDefaultSubobject<UBoxComponent>(TEXT("ChestCollision"));
	Chest->SetupAttachment(GetCapsuleComponent());

	// Inventory
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	State = ECharacterState::NONE;
	bCheatMode = false;
}

ECharacterState AMinecraftCharacter::GetState() const
{
	return State;
}

// Called when the game starts or when spawned
void AMinecraftCharacter::BeginPlay()
{
	Super::BeginPlay();
	bPaused = false;
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputLocalPlayerSubsystem();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(BaseInputMapping, 0);
}

// Called every frame
void AMinecraftCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePosition();
}

// Called to bind functionality to input
void AMinecraftCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UMinecraftInputComponent* MIC = Cast<UMinecraftInputComponent>(PlayerInputComponent);
	check(MIC);

	const FMinecraftGameplayTags& GameplayTags = FMinecraftGameplayTags::Get();

	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_LeftHand, ETriggerEvent::Started, this, &ThisClass::InputLeftHand);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_RightHand, ETriggerEvent::Started, this, &ThisClass::InputRightHand);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Fly, ETriggerEvent::Triggered, this, &ThisClass::InputFly);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Swim, ETriggerEvent::Triggered, this, &ThisClass::InputSwim);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Started, this, &ThisClass::StartCrouch);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Completed, this, &ThisClass::EndCrouch);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Canceled, this, &ThisClass::EndCrouch);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_FlyMode, ETriggerEvent::Started, this, &ThisClass::InputCheatMode);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_Menu, ETriggerEvent::Started, this, &ThisClass::ShowMenu);
	MIC->BindActionByTag(InputConfig, GameplayTags.InputTag_ScrollItem, ETriggerEvent::Started, InventoryComponent.Get(),
	                     &UInventoryComponent::HandleWheelScroll);
}

void AMinecraftCharacter::UpdatePosition()
{
	FVector NewWorldPosition = Feet->GetComponentLocation() / BLOCK_SIZE;
	FIntVector NewPosition(NewWorldPosition);
	FIntPoint NewChunkPosition(NewPosition.X / CHUNK_SIZE, NewPosition.Y / CHUNK_SIZE);

	NewChunkPosition.X += NewWorldPosition.X < 0 ? -1 : 0;
	NewChunkPosition.Y += NewWorldPosition.Y < 0 ? -1 : 0;

	NewPosition.X += NewWorldPosition.X < 0 ? -1 : 0;
	NewPosition.Y += NewWorldPosition.Y < 0 ? -1 : 0;
	NewPosition.Z += NewWorldPosition.Z < 0 ? -1 : 0;

	OnWorldPositionChange.ExecuteIfBound(NewWorldPosition, NewWorldPosition - WorldPosition);
	WorldPosition = NewWorldPosition;

	if (NewPosition == BlockPosition)
	{
		return;
	}

	OnBlockPositionChange.ExecuteIfBound(NewPosition, NewPosition - BlockPosition);

	BlockPosition = NewPosition;

	if (NewChunkPosition != ChunkPosition)
	{
		OnChunkPositionChange.ExecuteIfBound(NewChunkPosition, NewChunkPosition - ChunkPosition);
		ChunkPosition = NewChunkPosition;
	}
}

void AMinecraftCharacter::InputMove(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X * 0.5);
}

void AMinecraftCharacter::InputLook(const FInputActionValue& InputActionValue)
{
	FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AMinecraftCharacter::StartCrouch()
{
	Crouch();
}

void AMinecraftCharacter::EndCrouch()
{
	UnCrouch();
}

void AMinecraftCharacter::InputLeftHand(const FInputActionValue& InputActionValue)
{
	FHitResult OutHit;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation(); // GetActorLocation();
	FRotator Rotation = GetController()->GetControlRotation();
	FVector ForwardVector = UE::Geometry::Normalized(UKismetMathLibrary::GetForwardVector(Rotation));
	FVector End = ((ForwardVector * ReachDistance) + Start);
	FCollisionQueryParams CollisionParams;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.2, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		OnMine.ExecuteIfBound(OutHit.ImpactPoint + 0.1 * UE::Geometry::Normalized(OutHit.ImpactPoint - Start));
	}
}

void AMinecraftCharacter::InputRightHand(const FInputActionValue& InputActionValue)
{
	FHitResult OutHit;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation(); // GetActorLocation();
	FRotator Rotation = GetController()->GetControlRotation();
	FVector ForwardVector = UE::Geometry::Normalized(UKismetMathLibrary::GetForwardVector(Rotation));
	FVector End = ((ForwardVector * ReachDistance) + Start);
	FCollisionQueryParams CollisionParams;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.2, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		const UBlockItem* BlockItem = InventoryComponent->GetActiveItem();
		if (!BlockItem)
		{
			return;
		}
		const EBlockType Type = BlockItem->Type;
		if (Type == EBlockType::AIR)
		{
			return;
		}
		OnPlace.ExecuteIfBound(OutHit.ImpactPoint - 0.1 * UE::Geometry::Normalized(OutHit.ImpactPoint - Start), Type);
	}
}

void AMinecraftCharacter::InputFly(const FInputActionValue& InputActionValue)
{
	float Amount = InputActionValue.Get<float>() * FlyingFactor;
	AddMovementInput(GetActorUpVector(), Amount);
}

void AMinecraftCharacter::InputSwim(const FInputActionValue& InputActionValue)
{
	float Amount = InputActionValue.Get<float>() * SwimmingFactor;
	AddMovementInput(GetActorUpVector(), Amount);
}

void AMinecraftCharacter::InputCheatMode(const FInputActionValue& InputActionValue)
{
	if (bCheatMode)
	{
		EndCheatMode();
		return;
	}
	StartCheatMode();
}

void AMinecraftCharacter::StartSwimming()
{
	State = ECharacterState::SWIMMING;

	if (bCheatMode)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputLocalPlayerSubsystem();
	check(Subsystem);

	if (ActiveInputMapping)
	{
		Subsystem->RemoveMappingContext(ActiveInputMapping);
	}
	Subsystem->AddMappingContext(SwimmingInputMapping, 1);
	ActiveInputMapping = SwimmingInputMapping;

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	MovementComponent->SetMovementMode(MOVE_Swimming);
	MovementComponent->GetPhysicsVolume()->bWaterVolume = true;
}

void AMinecraftCharacter::StartWalkingInWater()
{
	State = ECharacterState::WATERWALKING;

	if (bCheatMode)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputLocalPlayerSubsystem();
	check(Subsystem);

	if (ActiveInputMapping)
	{
		Subsystem->RemoveMappingContext(ActiveInputMapping);
	}
	Subsystem->AddMappingContext(WalkingInputMapping, 1);
	ActiveInputMapping = WalkingInputMapping;

	MovementComponent->SetMovementMode(MOVE_Walking);
	MovementComponent->GetPhysicsVolume()->bWaterVolume = false;

	MovementComponent->MaxWalkSpeed = WaterWalkingSpeed;
}

void AMinecraftCharacter::StartWalking()
{
	State = ECharacterState::WALKING;

	if (bCheatMode)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputLocalPlayerSubsystem();
	check(Subsystem);

	if (ActiveInputMapping)
	{
		Subsystem->RemoveMappingContext(ActiveInputMapping);
	}
	Subsystem->AddMappingContext(WalkingInputMapping, 1);
	ActiveInputMapping = WalkingInputMapping;

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	MovementComponent->SetMovementMode(MOVE_Walking);
	MovementComponent->GetPhysicsVolume()->bWaterVolume = false;
	MovementComponent->MaxWalkSpeed = WalkingSpeed;
}

void AMinecraftCharacter::StartCheatMode()
{
	bCheatMode = true;
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputLocalPlayerSubsystem();
	check(Subsystem);

	if (ActiveInputMapping)
	{
		Subsystem->RemoveMappingContext(ActiveInputMapping);
	}

	Subsystem->AddMappingContext(FlyingInputMapping, 1);
	ActiveInputMapping = FlyingInputMapping;

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	MovementComponent->SetMovementMode(MOVE_Flying);
	MovementComponent->GetPhysicsVolume()->bWaterVolume = false;
}

void AMinecraftCharacter::EndCheatMode()
{
	bCheatMode = false;
	if (State == ECharacterState::WALKING)
	{
		StartWalking();
		return;
	}
	StartSwimming();
}

FVector AMinecraftCharacter::GetEyePosition() const
{
	return FirstPersonCameraComponent->GetComponentLocation();
}

FVector AMinecraftCharacter::GetChestPosition() const
{
	return Chest->GetComponentLocation();
}

FVector AMinecraftCharacter::GetFeetPosition() const
{
	return Feet->GetComponentLocation();
}

void AMinecraftCharacter::ShowMenu()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	AMinecraftHUD* HUD = PlayerController->GetHUD<AMinecraftHUD>();
	if (!bPaused)
	{
		HUD->ShowPauseMenu();
		bPaused = true;
	}
	else
	{
		HUD->HidePauseMenu();
		bPaused = false;
	}
}

UEnhancedInputLocalPlayerSubsystem* AMinecraftCharacter::GetEnhancedInputLocalPlayerSubsystem()
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	}
	return nullptr;
}

ULocalPlayer* AMinecraftCharacter::GetLocalPlayer()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		return PlayerController->GetLocalPlayer();
	}
	return nullptr;
}
