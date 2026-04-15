// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftWorld.h"

#include "MinecraftWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Minecraft/MinecraftGameInstance.h"
#include "Minecraft/MinecraftGameMode.h"
#include "Minecraft/MinecraftGameState.h"
#include "Minecraft/MinecraftSaveGame.h"
#include "Minecraft/Character/MinecraftCharacter.h"
#include "Minecraft/UI/MinecraftHUD.h"


// Sets default values
AMinecraftWorld::AMinecraftWorld()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMinecraftWorld::BeginPlay()
{
	Super::BeginPlay();
	AMinecraftGameMode* GameMode = Cast<AMinecraftGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->MinecraftWorld = this;
	bDebugScreen = false;
	SpawnWorld();
}

void AMinecraftWorld::HandlePlayerChangedBlockPosition(const FIntVector& PlayerBlockPosition, const FIntVector& PositionOffset)
{
	const FBlock& FeetBlock = ChunkMap->GetBlock(MinecraftCharacter->GetFeetPosition());
	const FBlock& ChestBlock = ChunkMap->GetBlock(MinecraftCharacter->GetChestPosition());

	APlayerController* PlayerController = Cast<APlayerController>(MinecraftCharacter->GetController());
	AMinecraftHUD* HUD = PlayerController->GetHUD<AMinecraftHUD>();
	HUD->GetDebugScreen()->SetPlayerPositionText(PlayerBlockPosition);
	HUD->GetDebugScreen()->SetBlockLightLevelText(FeetBlock.LightValue);
	HUD->GetDebugScreen()->SetBlockIDText(FeetBlock.ID);

	if (ChestBlock.State == EBlockState::LIQUID)
	{
		if (MinecraftCharacter->GetState() != ECharacterState::SWIMMING)
		{
			MinecraftCharacter->StartSwimming();
		}
	}
	else if (FeetBlock.State == EBlockState::LIQUID)
	{
		if (MinecraftCharacter->GetState() != ECharacterState::WATERWALKING)
		{
			MinecraftCharacter->StartWalkingInWater();
		}
	}
	else
	{
		if (MinecraftCharacter->GetState() != ECharacterState::WALKING)
		{
			MinecraftCharacter->StartWalking();
		}
	}
}

void AMinecraftWorld::HandlePlayerChangedWorldPosition(const FVector& PlayerWorldPosition, const FVector& PositionOffset)
{
	const FBlock& EyeBlock = ChunkMap->GetBlock(MinecraftCharacter->GetEyePosition());
	UnderWaterPostProcessVolume->bEnabled = EyeBlock.State == EBlockState::LIQUID;

	const auto CVarFog = IConsoleManager::Get().FindConsoleVariable(TEXT("mc.Fog"));
	bool Value = CVarFog->GetBool();

	FogPostProcessVolume->bEnabled = !UnderWaterPostProcessVolume->bEnabled && Value;
}

void AMinecraftWorld::HandlePlayerMine(const FVector& BlockPosition)
{
	ChunkMap->RemoveBlock(BlockPosition);
}

void AMinecraftWorld::HandlePlayerPlace(const FVector& BlockPosition, EBlockType Type)
{
	FIntVector PlayersFeetBlockPosition(MinecraftCharacter->GetFeetPosition() / BLOCK_SIZE);

	PlayersFeetBlockPosition.X += PlayersFeetBlockPosition.X < 0 ? -1 : 0;
	PlayersFeetBlockPosition.Y += PlayersFeetBlockPosition.Y < 0 ? -1 : 0;
	PlayersFeetBlockPosition.Z += PlayersFeetBlockPosition.Z < 0 ? -1 : 0;

	ChunkMap->AddBlock(Type, BlockPosition, PlayersFeetBlockPosition);
}

UBlocksDatabase* AMinecraftWorld::GetBlockDatabase()
{
	return BlocksDatabase;
}

void AMinecraftWorld::SaveWorld()
{
	UMinecraftSaveGame* SaveObject = Cast<UMinecraftSaveGame>(UGameplayStatics::CreateSaveGameObject(UMinecraftSaveGame::StaticClass()));
	SaveObject->PlayerPosition = MinecraftCharacter->GetActorLocation();
	SaveObject->PlayerRotation = MinecraftCharacter->GetControlRotation();
	SaveObject->Seed = WorldSeed;

	AMinecraftGameState* GameState = Cast<AMinecraftGameState>(GetWorld()->GetGameState());

	auto& AlteredBlocks = GameState->AlteredBlocks;

	for (auto& Chunk : AlteredBlocks)
	{
		for (auto& Block : Chunk.Value)
		{
			SaveObject->AlteredBlocksChunkPosition.Add(Chunk.Key);
			SaveObject->AlteredBlocksBlockPosition.Add(Block.Key);
			SaveObject->AlteredBlocksBlockType.Add(Block.Value);
		}
	}
	UGameplayStatics::SaveGameToSlot(SaveObject, WorldName.ToString(), 0);
}

void AMinecraftWorld::LoadWorld(const FString& Name)
{
	UMinecraftSaveGame* SaveObject = Cast<UMinecraftSaveGame>(UGameplayStatics::LoadGameFromSlot(Name, 0));
	TMap<FIntPoint, TMap<FIntVector, const EBlockType>> AlteredBlocks;

	for (int i = 0; i < SaveObject->AlteredBlocksChunkPosition.Num(); i++)
	{
		FIntPoint ChunkPosition = SaveObject->AlteredBlocksChunkPosition[i];
		FIntVector BlockPosition = SaveObject->AlteredBlocksBlockPosition[i];
		EBlockType Type = SaveObject->AlteredBlocksBlockType[i];

		TMap<FIntVector, const EBlockType>* Blocks = AlteredBlocks.Find(ChunkPosition);

		if (!Blocks)
		{
			Blocks = &AlteredBlocks.Add(ChunkPosition);
		}
		Blocks->Add(BlockPosition, Type);
	}
	AMinecraftGameState* GameState = Cast<AMinecraftGameState>(GetWorld()->GetGameState());
	GameState->AlteredBlocks = AlteredBlocks;
	MinecraftCharacter->SetActorLocation(SaveObject->PlayerPosition);
	MinecraftCharacter->GetController()->SetControlRotation(SaveObject->PlayerRotation);

	WorldSeed = SaveObject->Seed;
	WorldName = FName(Name);
}

void AMinecraftWorld::CreateWorld(const FString& Name, int Seed)
{
	this->WorldSeed = Seed;
	this->WorldName = FName(Name);
}

void AMinecraftWorld::HandlePlayerChangedChunkPosition(const FIntPoint& PlayerChunkPosition, const FIntPoint& PositionOffset)
{
	ChunkMap->ChangeCenter(PlayerChunkPosition);
}

// Called every frame
void AMinecraftWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto CVarDebugScreen = IConsoleManager::Get().FindConsoleVariable(TEXT("mc.DebugScreen"));
	bool Value = CVarDebugScreen->GetBool();
	
	if (!bDebugScreen && Value)
	{
		ToggleDebugScreen();
		bDebugScreen = true;
	}
	else if(bDebugScreen && !Value)
	{
		ToggleDebugScreen();
		bDebugScreen = false;
	}
}

uint8 AMinecraftWorld::GetRenderDistance() const
{
	return RenderDistance;
}

void AMinecraftWorld::ChangeRenderDistance(uint8 NewRenderDistance)
{
	if (NewRenderDistance > MAX_RENDER_DISTANCE || NewRenderDistance < MIN_RENDER_DISTANCE)
	{
		return;
	}

	if (NewRenderDistance == RenderDistance)
	{
		return;
	}

	RenderDistance = NewRenderDistance;
	ChunkMap->ChangeRadius(RenderDistance);
	DynamicFogMaterial->SetScalarParameterValue("FogOffset", (RenderDistance - 3) * CHUNK_SIZE * BLOCK_SIZE);
}

void AMinecraftWorld::RenderDistanceUp()
{
	if (RenderDistance < MAX_RENDER_DISTANCE)
	{
		RenderDistance++;
		ChunkMap->ChangeRadius(RenderDistance);
		DynamicFogMaterial->SetScalarParameterValue("FogOffset", (RenderDistance - 3) * CHUNK_SIZE * BLOCK_SIZE);
	}
}

void AMinecraftWorld::RenderDistanceDown()
{
	if (RenderDistance > MIN_RENDER_DISTANCE)
	{
		RenderDistance--;
		ChunkMap->ChangeRadius(RenderDistance);
		DynamicFogMaterial->SetScalarParameterValue("FogOffset", (RenderDistance - 3) * CHUNK_SIZE * BLOCK_SIZE);
	}
}

void AMinecraftWorld::ToggleDebugScreen()
{
	APlayerController* PlayerController = Cast<APlayerController>(MinecraftCharacter->GetController());
	AMinecraftHUD* HUD = PlayerController->GetHUD<AMinecraftHUD>();
	HUD->ToggleDebugScreen();
}

void AMinecraftWorld::SpawnWorld()
{
	APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	MinecraftCharacter = Cast<AMinecraftCharacter>(Pawn);
	MinecraftCharacter->OnBlockPositionChange.BindUObject(this, &ThisClass::HandlePlayerChangedBlockPosition);
	MinecraftCharacter->OnChunkPositionChange.BindUObject(this, &ThisClass::HandlePlayerChangedChunkPosition);
	MinecraftCharacter->OnWorldPositionChange.BindUObject(this, &ThisClass::HandlePlayerChangedWorldPosition);
	MinecraftCharacter->OnMine.BindUObject(this, &ThisClass::HandlePlayerMine);
	MinecraftCharacter->OnPlace.BindUObject(this, &ThisClass::HandlePlayerPlace);

	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());
	RenderDistance = GameInstance->RenderDistance;

	if (!GameInstance->bNewWorld)
	{
		LoadWorld(GameInstance->WorldName);
	}
	else
	{
		CreateWorld(GameInstance->WorldName, GameInstance->WorldSeed);
	}

	BlocksDatabase = NewObject<UBlocksDatabase>(this, BlocksDatabaseClass);
	BlocksDatabase->Init();

	WorldGenerator = NewObject<UWorldGenerator>(this, WorldGeneratorClass);
	WorldGenerator->Init(WorldSeed);

	WorldBuilder = Cast<AWorldBuilder>(GetWorld()->SpawnActor(WorldBuilderClass));
	WorldBuilder->Init(BlocksDatabase, WorldGenerator);

	FIntVector NewPosition = FIntVector(Pawn->GetActorLocation()) / BLOCK_SIZE;
	FIntPoint NewChunkPosition(NewPosition.X / CHUNK_SIZE, NewPosition.Y / CHUNK_SIZE);
	NewChunkPosition.X += NewPosition.X < 0 ? -1 : 0;
	NewChunkPosition.Y += NewPosition.Y < 0 ? -1 : 0;

	ChunkMap = Cast<AChunkMap>(GetWorld()->SpawnActor(AChunkMap::StaticClass()));
	ChunkMap->Init(RenderDistance, NewChunkPosition, WorldBuilder, BlocksDatabase);
	ChunkMap->Spawn();

	UMaterial* FogMaterial = Cast<UMaterial>(FogPostProcessVolume->Settings.WeightedBlendables.Array[0].Object);
	DynamicFogMaterial = UMaterialInstanceDynamic::Create(FogMaterial, this);
	FogPostProcessVolume->Settings.WeightedBlendables.Array[0].Object = DynamicFogMaterial;
	DynamicFogMaterial->SetScalarParameterValue("FogOffset", (RenderDistance - 3) * CHUNK_SIZE * BLOCK_SIZE);
	DynamicFogMaterial->SetScalarParameterValue("FogThickness", 2 * CHUNK_SIZE * BLOCK_SIZE);
	DynamicFogMaterial->SetScalarParameterValue("FogMaxRange", 100 * CHUNK_SIZE * BLOCK_SIZE);
}
