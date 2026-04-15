// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "WorldGenerator.h"
#include "GameFramework/Actor.h"
#include "WorldBuilder.generated.h"


UCLASS(Blueprintable)
class MINECRAFT_API AWorldBuilder : public AActor
{
	GENERATED_BODY()
	DECLARE_DELEGATE_OneParam(FChunkBuiltDelegate, AChunk* Chunk)

public:
	AWorldBuilder();
	
	/*
	 * Initialize the world builder. It need BlockDatabse for generating blocks and a noise generator
	 */
	void Init(UBlocksDatabase* Manager, UWorldGenerator* Generator);
	
	/*
	 * Takes chunk and populates it with blocks
	 */
	AChunk* BuildChunk(AChunk* Chunk);

	/*
	 * Calculates density of a block based on 3D noise map. The noise is then scaled according to the height of the blocks column
	 */
	float CalculateDensity(int X, int Y, int Z, int Height);

public:
	FChunkBuiltDelegate OnChunkBuilt;

private:
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<UWorldGenerator> WorldGenerator;
	UPROPERTY()
	TObjectPtr<UBlocksDatabase> BlockManager;
};
