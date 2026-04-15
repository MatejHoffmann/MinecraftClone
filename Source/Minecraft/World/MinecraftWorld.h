// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlocksDatabase.h"
#include "ChunkMap.h"
#include "WorldBuilder.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/Actor.h"
#include "Minecraft/Character//MinecraftCharacter.h"
#include "MinecraftWorld.generated.h"

UCLASS()
class MINECRAFT_API AMinecraftWorld : public AActor
{
	GENERATED_BODY()

public:
	
	/*
	* Sets default values for this actor's properties
	*/
	AMinecraftWorld();
	
	/*
	 * Called every frame
	 */
	virtual void Tick(float DeltaTime) override;
	
	/*
	 * Change render distance by +1
	 */
	UFUNCTION(BlueprintCallable)
	uint8 GetRenderDistance() const;

	/*
	 * Change render distance by +1
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeRenderDistance(uint8 NewRenderDistance);

	/*
	 * Change render distance by +1
	 */
	UFUNCTION(BlueprintCallable)
	void RenderDistanceUp();

	/*
	 * Change render distance by -1
	 */
	UFUNCTION(BlueprintCallable)
	void RenderDistanceDown();

	/*
	 * Toggle visibility of the debug screen
	 */
	void ToggleDebugScreen();

	/*
	 * Spawns the world and initializes all necesarry variables
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnWorld();

	/*
	 * Save current state of the world
	 */
	UFUNCTION(BlueprintCallable)
	void SaveWorld();

	/*
	 * Load world from file with the given name
	 */
	UFUNCTION(BlueprintCallable)
	void LoadWorld(const FString& Name);

	/*
	 * Create new world
	 */
	UFUNCTION(BlueprintCallable)
	void CreateWorld(const FString& Name, int Seed);

	/*
	 * Get block database
	 */
	UBlocksDatabase* GetBlockDatabase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/*
	 * Handle when player changes block position (used for setting player movement based on environment)
	 */
	void HandlePlayerChangedBlockPosition(const FIntVector& PlayerBlockPosition, const FIntVector& PositionOffset);

	/*
	 * Handle when player changes chunk position (used for updating the chunk map)
	 */
	void HandlePlayerChangedChunkPosition(const FIntPoint& PlayerChunkPosition, const FIntPoint& PositionOffset);

	/*
	 * Handle when player changes position (used for setting the correct post processing)
	 */
	void HandlePlayerChangedWorldPosition(const FVector& PlayerWorldPosition, const FVector& PositionOffset);
	/*
	 * Handle when player want's to remove a block
	 */
	void HandlePlayerMine(const FVector& BlockPosition);

	/*
	 * Handle when player want's to place a block
	 */
	void HandlePlayerPlace(const FVector& BlockPosition, EBlockType Type);

private:
	// Settings
	UPROPERTY()
	FName WorldName;
	UPROPERTY()
	int64 WorldSeed;
	UPROPERTY(EditAnywhere, Category="Minecraft|Settings")
	uint8 RenderDistance;

	UPROPERTY(EditDefaultsOnly, Category="Minecraft|Defaults")
	TSubclassOf<UBlocksDatabase> BlocksDatabaseClass;
	UPROPERTY()
	TObjectPtr<UBlocksDatabase> BlocksDatabase;

	UPROPERTY(EditDefaultsOnly, Category="Minecraft|Defaults")
	TSubclassOf<UWorldGenerator> WorldGeneratorClass;
	UPROPERTY()
	TObjectPtr<UWorldGenerator> WorldGenerator;

	UPROPERTY(EditDefaultsOnly, Category="Minecraft|Defaults")
	TSubclassOf<AWorldBuilder> WorldBuilderClass;
	UPROPERTY()
	TObjectPtr<AWorldBuilder> WorldBuilder;

	bool bDebugScreen;

	// Post processing
	UPROPERTY(EditAnywhere, Category="Minecraft|Post Processing")
	TObjectPtr<APostProcessVolume> UnderWaterPostProcessVolume;
	UPROPERTY(EditAnywhere, Category="Minecraft|Post Processing")
	TObjectPtr<APostProcessVolume> FogPostProcessVolume;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicFogMaterial;

	UPROPERTY()
	TObjectPtr<AMinecraftCharacter> MinecraftCharacter;

	// Chunks
	UPROPERTY()
	TObjectPtr<AChunkMap> ChunkMap;
};
