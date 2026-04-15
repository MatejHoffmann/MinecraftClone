// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MinecraftWorldSettings.h"
#include "BlocksDatabase.h"
#include "Chunk.generated.h"

class AChunkMap;

UCLASS()
class MINECRAFT_API AChunk : public AActor
{
	GENERATED_BODY()

	// Section data
	enum class ESectionState
	{
		EMPTY,
		MESHED,
		DIRTY,
	};

	struct FSection
	{
		FSection(): State(ESectionState::EMPTY)
		{
		}

		ESectionState State;

		TArray<FVector> SolidBlocksVertexBuffer;
		TArray<int32> SolidBlocksTriangleBuffer;
		TArray<FVector2D> SolidBlocksUVBuffer;
		TArray<FColor> SolidBlocksVertexColorBuffer;

		TArray<FVector> LiquidBlocksVertexBuffer;
		TArray<int32> LiquidBlocksTriangleBuffer;
		TArray<FVector2D> LiquidBlocksUVBuffer;
		TArray<FColor> LiquidBlocksVertexColorBuffer;
	};

	enum class EChunkState
	{
		EMPTY,
		GENERATING,
		GENERATED,
		MESHING,
		MESHED,
		DIRTY
	};

private:
	DECLARE_DELEGATE_OneParam(FChunkGeneratedDelegate, AChunk* Chunk)
	DECLARE_DELEGATE_OneParam(FChunkMeshedDelegate, AChunk* Chunk)
	DECLARE_DELEGATE_OneParam(FChunkDirtyDelegate, AChunk* Chunk);

	/*
	 * Create vertex in the world coordinates based on the local vertex coordinate
	 */
	FVector CreateVertex(const FIntVector& VertexCoordinates) const;

	/*
	 * Create face of the block in the given direction
	 */
	void CreateBlockFace(const FIntVector& BlockPosition, EMinecraftDirection Direction,
	                     TArray<FVector>& VertexBuffer, TArray<int32>& TriangleBuffer, TArray<FVector2D>& UVBuffer, TArray<FColor>& ColorBuffer);

	/*
	 * Calculate per block lightening
	 */
	void PropagateLightInChunk();

	/*
	 * Blend light between two neighbor chunks
	 */
	void BlendLightWithNeighborChunk(EMinecraftDirection Direction);

	/*
	 * Propagate light from surrounding chunks, thus creating correct lighting 
	 */
	void PropagateLightFromNeighborChunks();

	/*
	 * Propagate given array of light sources through chunk
	 */
	void PropagateLightSourcesInChunk(TQueue<FIntVector>& LightSources);

	/*
	 * Calculates world position from the given local position
	 */
	FIntVector GetWorldPositionFromLocal(const FIntVector& LocalPosition) const;

	/*
	 * Build mesh of the given section
	 */
	void BuildSectionMesh(int SectionID);

	/*
	 * Build mesh of a block
	 */
	void BuildBlockMesh(int SectionID, const FIntVector& LocalBlockPosition);

	/*
	 * Create Chunk border mesh for debugging 
	 */
	void CreateChunkBorder();

	/*
	 * Check whether the block position is out of chunk bounds
	 */
	bool IsBlockOutOfChunkBounds(const FIntVector& BlockPosition) const;

	/*
	 * Mark chunk section as dirty
	 */
	void SetSectionDirty(int32 SectionID);

	/*
	 * Mark the chunk as dirty and signal the corresponding delegate
	 */
	void SetChunkDirty();

	/*
	 * Get value of the highest point of the chunk
	 */
	int32 GetMaxHeight() const;

public:
	AChunk();

	/*
	 * Restart the chunk to its initial state
	 */
	void Restart();
	
	/*
	 * Initialize new chunk
	 */
	void Init(const FIntPoint& ChunkPosition, UBlocksDatabase* Database, const AChunkMap* ChunkMap);

	/*
	 * Set chunk state to generated
	 */
	void SetChunkStateAsGenerated();

	/*
	 * Returns whether the chunk is generated or nor 
	 */
	bool IsGenerated() const;
	/*
	 * Returns whether the chunk is meshed or not
	 */
	bool IsMeshed() const;

	/*
	 * Returns whether the chunk is dirty
	 */
	bool IsDirty() const;

	/*
	 * Returns whether the chunk is empty
	 */
	bool IsEmpty() const;

	/*
	 * Returns whether the mesh is visible 
	 */
	bool IsMeshVisible() const;

	/*
	 * Hide mesh of this chunk
	 */
	void HideMesh();

	/*
	 * show mesh of this chunk
	 */
	void ShowMesh();

	/*
	 * Show Chunk border
	 */
	void ShowBorder();

	/*
	 * Hide Chunk border
	 */
	void HideBorder();

	/*
	 * Build mesh of this chunk
	 */
	void BuildMesh();

	/*
	 * Destroy mesh of this chunk
	 */
	void DestroyMesh();

	/*
	 * Push the mesh to the renderer (Must be done in the main thread)
	 */
	void RenderMesh();

	/*
	 * Set block at the given position to the given block
	 */
	void SetBlock(const FIntVector& BlockPosition, const FBlock& Block);

	/*
	 * Get block at the given position
	 */
	const FBlock& GetBlock(const FIntVector& LocalBlockPosition) const;
	FBlock& GetBlock(const FIntVector& LocalBlockPosition);

	/*
	 * Get world position of this chunk
	 */
	FIntPoint GetPosition() const;

	/*
	 * Get Height of the chunk at the given position
	 */
	int32 GetHeight(const FIntPoint& LocalBlockPosition) const;

	/*
	 * Returns lock of this chunk
	 */
	FCriticalSection& GetLock();

protected:
	virtual void BeginPlay() override;

public:
	FChunkGeneratedDelegate OnGenerated;
	FChunkMeshedDelegate OnMeshed;
	FChunkDirtyDelegate OnDirty;

private:
	UPROPERTY(VisibleAnywhere, Category="Minecraft")
	FIntPoint Position;
	EChunkState State;
	FCriticalSection Lock;

	UPROPERTY()
	TObjectPtr<UBlocksDatabase> BlockDatabase;

	UPROPERTY()
	TObjectPtr<const AChunkMap> ChunkMap;

	UPROPERTY()
	TMap<EMinecraftDirection, bool> LightPropagatedFromNeighbor;

	// Geometry data
	TArray<FSection> Sections;

	UPROPERTY()
	TArray<int32> Heights;

	UPROPERTY()
	TArray<FBlock> Blocks;

	// Mesh data
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> SolidBlocksMesh;
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> LiquidBlocksMesh;
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> ChunkBorderMesh;

	UPROPERTY()
	TArray<FVector> SolidBlocksVertexBuffer;
	UPROPERTY()
	TArray<int32> SolidBlocksTriangleBuffer;
	UPROPERTY()
	TArray<FVector2D> SolidBlocksUVBuffer;
	UPROPERTY()
	TArray<FColor> SolidBlocksVertexColorBuffer;

	UPROPERTY()
	TArray<FVector> LiquidBlocksVertexBuffer;
	UPROPERTY()
	TArray<int32> LiquidBlocksTriangleBuffer;
	UPROPERTY()
	TArray<FVector2D> LiquidBlocksUVBuffer;
	UPROPERTY()
	TArray<FColor> LiquidBlocksVertexColorBuffer;

	UPROPERTY()
	TArray<FVector> ChunkBorderVertexBuffer;
	UPROPERTY()
	TArray<int32> ChunkBorderTriangleBuffer;
};
