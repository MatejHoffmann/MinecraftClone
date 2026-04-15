// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldBuilder.h"
#include "GameFramework/Actor.h"
#include "Minecraft/Utils/PriorityQueue.h"
#include "ChunkMap.generated.h"

class AChunk;

UCLASS()
class MINECRAFT_API AChunkMap : public AActor
{
	GENERATED_BODY()

public:
	AChunkMap();
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

	/*
	 * Initialize the chunk map
	 */
	void Init(uint8 MapRadius, const FIntPoint& MapCenter, AWorldBuilder* Builder, UBlocksDatabase* Database);

	/*
	 * Spawn the chunk map
	 */
	void Spawn();

	/*
	 * Change center of the chunk map
	 */
	void ChangeCenter(const FIntPoint& NewCenter);
	
	/*
	 * Change radius of the chunk map
	 */
	void ChangeRadius(uint8 NewRadius);

	/*
	 * Add new block of the given type to the map
	 */
	void AddBlock(EBlockType Type, const FVector& WorldPosition, const FIntVector& PlayerBlockPosition);

	/*
	 * Remove block from the map at the given position
	 */
	void RemoveBlock(const FVector& WorldPosition);

	/*
	 * Get block at the given world position
	 */
	const FBlock& GetBlock(const FVector& BlockPosition);

	/*
	 * Toggle chunk border visibility
	 */
	void ToggleChunkBorderVisibility();

	/*
	 * Returns chunk that is right next to the given chunk in the given direction. If such chunk doesnt exist, returns nullptr
	 */
	AChunk* GetNeighborChunk(const FIntPoint& ChunkPosition, EMinecraftDirection Direction) const;
	AChunk* GetNeighborChunk(const AChunk* Chunk, EMinecraftDirection Direction) const;

	/*
	 * Get chunk at the given world position
	 */
	AChunk* GetChunk(const FIntPoint& ChunkPosition) const;

	/*
	 * Returns block that is right next to the given block in the given direction
	 */
	FBlock& GetNeighborBlock(const FIntVector& WorldBlockPosition, EMinecraftDirection Direction) const;
	FBlock& GetNeighborBlock(const FIntPoint& ChunkPosition, const FIntVector& LocalBlockPosition, EMinecraftDirection Direction) const;

	/*
	 * Get block at the given position
	 */
	FBlock& GetBlock(const FIntVector& WorldBlockPosition) const;
	FBlock& GetBlock(const FIntPoint& ChunkPosition, const FIntVector& LocalBlockPosition) const;

protected:
	virtual void BeginPlay() override;

private:
	/*
	 * Create new chunk at the given position with the given distance from the center point (used for priority queue)
	 */
	void CreateChunk(const FIntPoint& LocalChunkPosition, const FIntPoint& WorldChunkPosition, int32 Distance);

	/*
	 * Reuse already existing chunk, saving memory allocations. 
	 */
	void ReuseChunk(AChunk* Chunk, const FIntPoint& LocalChunkPosition, const FIntPoint& WorldChunkPosition, int32 Priority);

	/*
	 * Delete chunk at the given position
	 */
	void DeleteChunk(AChunk* Chunk, const FIntPoint& LocalChunkPosition, const FIntPoint& WorldChunkPosition);

	/*
	 * Handle when chunks gets dirty (by placing new block or removing block form a chunk that already has a mesh)
	 */
	void HandleChunkDirty(AChunk* Chunk);

	/*
	 * Spawns N tasks that handle generation of the chunk
	 */
	void BuildTask(int32 N = 3);

	/*
	 * Spawns N tasks that handle creation of a mesh of a chunk
	 */
	void MeshingTask(int32 N = 5);

	/*
	 * Push N chunk meshes to the renderer
	 */
	void RenderTasks(int32 N);

	/*
	 * Checks whether the neighbor chunks are already generated, so the given chunk can be meshed properly
	 */
	bool AreNeighborsReady(AChunk* Chunk);

private:
	uint8 Radius;
	UPROPERTY(VisibleAnywhere, Category="Minecraft")
	FIntPoint Center;

	UPROPERTY()
	UBlocksDatabase* BlocksDatabase;

	UPROPERTY()
	TObjectPtr<AWorldBuilder> WorldBuilder;

	UPROPERTY()
	TMap<FIntPoint, AChunk*> LocalChunkMap;
	FCriticalSection LocalChunkMapLock;

	UPROPERTY()
	TMap<FIntPoint, AChunk*> WorldChunkMap;
	FCriticalSection WorldChunkMapLock;

	TArray<UE::Tasks::FTask> TaskArray;

	bool bChunkBorderVisibility;

	FCriticalSection BuildQueueLock;
	TAtomic<bool> bBuildQueueWorkerOn;
	FPriorityQueue<FString, AChunk*> BuildQueue;
	TArray<UE::Tasks::FTask> BuildTasks;

	FCriticalSection MeshingNeighborsLock;
	FCriticalSection MeshingQueueLock;
	TAtomic<bool> bMeshingQueueWorkerOn;
	FPriorityQueue<FString, AChunk*> MeshingQueue;
	TArray<UE::Tasks::FTask> MeshingTasks;

	FPriorityQueue<FString, AChunk*> RenderQueue;
	
	FCriticalSection VisibilityQueueLock;
	TQueue<TPair<AChunk*, bool>> VisibilityQueue;

	FCriticalSection DestroyMeshQueueLock;
	TQueue<AChunk*> DestroyMeshQueue;

	FCriticalSection UpdateLock;
};
