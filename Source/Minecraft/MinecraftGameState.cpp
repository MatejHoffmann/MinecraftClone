// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftGameState.h"

EBlockType AMinecraftGameState::GetAlteredBlock(const FIntPoint& ChunkPosition, const FIntVector& BlockPosition)
{
	FScopeLock AlteredBlocksScopeLock(&AlteredBlocksLock);
	const TMap<FIntVector, const EBlockType>* ChunkAlteredBlocks = AlteredBlocks.Find(ChunkPosition);
	if (ChunkAlteredBlocks)
	{
		const EBlockType* Type = AlteredBlocks[ChunkPosition].Find(BlockPosition);
		if (Type)
		{
			return *Type;
		}
	}
	return EBlockType::AIR;
}

void AMinecraftGameState::AlterBlock(const FIntPoint& ChunkPosition, const FIntVector& BlockPosition, EBlockType Type)
{
	FScopeLock AlteredBlocksScopeLock(&AlteredBlocksLock);
	TMap<FIntVector, const EBlockType>* ChunkAlteredBlocks = AlteredBlocks.Find(ChunkPosition);
	if (!ChunkAlteredBlocks)
	{
		ChunkAlteredBlocks = &AlteredBlocks.Add(ChunkPosition);
	}
	ChunkAlteredBlocks->Add(BlockPosition, Type);
}

bool AMinecraftGameState::IsBlockAltered(const FIntPoint& ChunkPosition, const FIntVector& BlockPosition)
{
	FScopeLock AlteredBlocksScopeLock(&AlteredBlocksLock);
	if (!AlteredBlocks.Contains(ChunkPosition))
	{
		return false;
	}
	return AlteredBlocks[ChunkPosition].Contains(BlockPosition);
}
