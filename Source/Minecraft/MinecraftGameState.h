// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "World/Block.h"
#include "MinecraftGameState.generated.h"

UCLASS()
class MINECRAFT_API AMinecraftGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/*
	 * Get altered block at the given position. If the block wasn't altered, returns empty block
	 */
	EBlockType GetAlteredBlock(const FIntPoint& ChunkPosition, const FIntVector& BlockPosition);

	/*
	 * Alter block at the given position to the given type
	 */
	void AlterBlock(const FIntPoint& ChunkPosition, const FIntVector& BlockPosition, EBlockType Type);

	/*
	 * Checks whether the block is altered
	 */
	bool IsBlockAltered(const FIntPoint& ChunkPosition, const FIntVector& BlockPosition);

public:
	TMap<FIntPoint, TMap<FIntVector, const EBlockType>> AlteredBlocks;
	FCriticalSection AlteredBlocksLock;
};
