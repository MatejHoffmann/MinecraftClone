// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldBuilder.h"
#include "Minecraft/MinecraftGameState.h"

// Sets default values
AWorldBuilder::AWorldBuilder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AWorldBuilder::Init(UBlocksDatabase* Manager, UWorldGenerator* Generator)
{
	BlockManager = Manager;
	WorldGenerator = Generator;
	World = GetWorld();
}

AChunk* AWorldBuilder::BuildChunk(AChunk* Chunk)
{
	FIntPoint ChunkPosition = Chunk->GetPosition();
	AMinecraftGameState* GameState = Cast<AMinecraftGameState>(World->GetGameState());

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			float BlockX = x + CHUNK_SIZE * ChunkPosition.X;
			float BlockY = y + CHUNK_SIZE * ChunkPosition.Y;

			int32 Height = WorldGenerator->GetHeight(BlockX, BlockY);
			int32 NDirtblocks = WorldGenerator->GetSurfaceLayerHeight(x,y);;
			bool FirstLayerBlock = true;
			
			for (int z = CHUNK_HEIGHT -1; z >=0; z--)
			{
				float Density = CalculateDensity(BlockX, BlockY, z, Height);
				
				FBlock Block;
				
				if (Density < 0)
				{
					if (z <= 48)
					{
						Block = BlockManager->GetBlockByType(EBlockType::WATER);
						//Chunk->SetBlock({x, y, z}, BlockManager->GetBlockByType(EBlockType::WATER));
						FirstLayerBlock = false;
						NDirtblocks = 0;
					}
					else
					{
						Block = BlockManager->GetBlockByType(EBlockType::AIR);
						//Chunk->SetBlock({x, y, z}, BlockManager->GetBlockByType(EBlockType::AIR));	
					}
					if(!FirstLayerBlock)
					{
						NDirtblocks--;
					}
				}
				else
				{
					if(FirstLayerBlock)
					{
						Block = BlockManager->GetBlockByType(EBlockType::GRASS);
						//Chunk->SetBlock({x, y, z}, BlockManager->GetBlockByType(EBlockType::GRASS));
						FirstLayerBlock = false;
					}
					else if(NDirtblocks > 0)
					{
						Block = BlockManager->GetBlockByType(EBlockType::DIRT);
						//Chunk->SetBlock({x, y, z}, BlockManager->GetBlockByType(EBlockType::DIRT));
						NDirtblocks--;
					}	
					else
					{
						Block = BlockManager->GetBlockByType(EBlockType::STONE);
						//Chunk->SetBlock({x, y, z}, BlockManager->GetBlockByType(EBlockType::STONE));
					}	
				}
				
				if (GameState->IsBlockAltered(ChunkPosition, {x, y, z}))
				{
					Chunk->SetBlock({x, y, z}, BlockManager->GetBlockByType(GameState->GetAlteredBlock(ChunkPosition, {x, y, z})));
				}
				else
				{
					Chunk->SetBlock({x, y, z}, Block);
				}
			}
		}
	}
	Chunk->SetChunkStateAsGenerated();
	OnChunkBuilt.ExecuteIfBound(Chunk);
	return Chunk;
}

float AWorldBuilder::CalculateDensity(int X, int Y, int Z, int Height)
{
	float Density = WorldGenerator->GetDensity(X, Y, Z);
	float BaseHeight = Height / 2.0f;
	float SquishingFactor = (BaseHeight - Z) / BaseHeight;
	// The density is adjusted with the height value. The lower the Z coordinate, the higher is the density;
	SquishingFactor = SquishingFactor > 0 ? SquishingFactor * 1.8 : SquishingFactor * 0.4;
	return Density + SquishingFactor;
}
