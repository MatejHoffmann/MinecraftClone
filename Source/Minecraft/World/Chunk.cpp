// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "BlocksDatabase.h"
#include "ChunkMap.h"


AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = false;

	SolidBlocksMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SolidBlocksMesh"));
	SolidBlocksMesh->bUseAsyncCooking = true;
	RootComponent = SolidBlocksMesh;

	LiquidBlocksMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LiquidBlocksMesh"));
	ChunkBorderMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ChunkBorderMesh"));

	Sections.Init(FSection(), CHUNK_SECTIONS);
	Blocks.Init(FBlock(), BLOCKS_PER_CHUNK);
	Heights.Init(-1, BLOCKS_PER_CHUNK_HEIGHT);

	for (auto& Direction : FourNeighborhoodDirections)
	{
		LightPropagatedFromNeighbor.Add(Direction, false);
	}
}

void AChunk::Restart()
{
	State = EChunkState::EMPTY;

	for (int i = 0; i < BLOCKS_PER_CHUNK_HEIGHT; i++)
	{
		Heights[i] = -1;
	}

	for (int i = 0; i < CHUNK_SECTIONS; i++)
	{
		Sections[i].State = ESectionState::EMPTY;

		Sections[i].SolidBlocksVertexBuffer.Empty();
		Sections[i].SolidBlocksTriangleBuffer.Empty();
		Sections[i].SolidBlocksUVBuffer.Empty();
		Sections[i].SolidBlocksVertexColorBuffer.Empty();

		Sections[i].LiquidBlocksVertexBuffer.Empty();
		Sections[i].LiquidBlocksTriangleBuffer.Empty();
		Sections[i].LiquidBlocksUVBuffer.Empty();
		Sections[i].LiquidBlocksVertexColorBuffer.Empty();
	}

	for (auto& Direction : FourNeighborhoodDirections)
	{
		LightPropagatedFromNeighbor[Direction] = false;
	}
	ChunkBorderTriangleBuffer.Reset();
	ChunkBorderVertexBuffer.Reset();

	OnDirty.Unbind();
	OnGenerated.Unbind();
	OnMeshed.Unbind();
}


void AChunk::Init(const FIntPoint& ChunkPosition, UBlocksDatabase* Database, const AChunkMap* Map)
{
	Position = ChunkPosition;
	BlockDatabase = Database;
	ChunkMap = Map;

	SolidBlocksMesh->SetMaterial(0, BlockDatabase->GetSolidBlocksMaterial());
	SolidBlocksMesh->SetRenderCustomDepth(true);

	LiquidBlocksMesh->SetMaterial(0, BlockDatabase->GetLiquidBlocksMaterial());
	LiquidBlocksMesh->SetRenderCustomDepth(true);
	LiquidBlocksMesh->SetCustomDepthStencilValue(1);

	ChunkBorderMesh->SetMaterial(0, BlockDatabase->GetChunkBorderMaterial());
	CreateChunkBorder();
}

void AChunk::BuildMesh()
{
	PropagateLightFromNeighborChunks();

	for (int i = 0; i < CHUNK_SECTIONS; i++)
	{
		if (Sections[i].State == ESectionState::DIRTY)
		{
			BuildSectionMesh(i);
		}
		if (Sections[i].State == ESectionState::MESHED)
		{
			int32 IndexOffsetSolidBlocks = SolidBlocksVertexBuffer.Num();
			int32 IndexOffsetLiquidBlocks = LiquidBlocksVertexBuffer.Num();

			SolidBlocksVertexBuffer.Append(Sections[i].SolidBlocksVertexBuffer);

			for (int j = 0; j < Sections[i].SolidBlocksTriangleBuffer.Num(); j++)
			{
				SolidBlocksTriangleBuffer.Add(IndexOffsetSolidBlocks + Sections[i].SolidBlocksTriangleBuffer[j]);
			}

			SolidBlocksUVBuffer.Append(Sections[i].SolidBlocksUVBuffer);
			SolidBlocksVertexColorBuffer.Append(Sections[i].SolidBlocksVertexColorBuffer);

			LiquidBlocksVertexBuffer.Append(Sections[i].LiquidBlocksVertexBuffer);

			for (int j = 0; j < Sections[i].LiquidBlocksTriangleBuffer.Num(); j++)
			{
				LiquidBlocksTriangleBuffer.Add(IndexOffsetLiquidBlocks + Sections[i].LiquidBlocksTriangleBuffer[j]);
			}

			LiquidBlocksUVBuffer.Append(Sections[i].LiquidBlocksUVBuffer);
			LiquidBlocksVertexColorBuffer.Append(Sections[i].LiquidBlocksVertexColorBuffer);
		}
	}

	State = EChunkState::MESHED;
	OnMeshed.ExecuteIfBound(this);
}

void AChunk::DestroyMesh()
{
	SolidBlocksMesh->ClearMeshSection(0);
	LiquidBlocksMesh->ClearMeshSection(0);
}

void AChunk::HideMesh()
{
	SolidBlocksMesh->SetVisibility(false);
	LiquidBlocksMesh->SetVisibility(false);
}

void AChunk::ShowMesh()
{
	SolidBlocksMesh->SetVisibility(true);
	LiquidBlocksMesh->SetVisibility(true);
}

void AChunk::ShowBorder()
{
	ChunkBorderMesh->SetVisibility(true);
}

void AChunk::HideBorder()
{
	ChunkBorderMesh->SetVisibility(false);
}

void AChunk::BuildSectionMesh(int SectionID)
{
	int HeightOffset = SectionID * CHUNK_SIZE;

	Sections[SectionID].SolidBlocksVertexBuffer.Reset();
	Sections[SectionID].SolidBlocksTriangleBuffer.Reset();
	Sections[SectionID].SolidBlocksUVBuffer.Reset();
	Sections[SectionID].SolidBlocksVertexColorBuffer.Reset();

	Sections[SectionID].LiquidBlocksVertexBuffer.Reset();
	Sections[SectionID].LiquidBlocksTriangleBuffer.Reset();
	Sections[SectionID].LiquidBlocksUVBuffer.Reset();
	Sections[SectionID].LiquidBlocksVertexColorBuffer.Reset();

	for (int X = 0; X < CHUNK_SIZE; X++)
	{
		for (int Y = 0; Y < CHUNK_SIZE; Y++)
		{
			int32 HeighIndex = Y + X * CHUNK_SIZE;

			for (int Z = HeightOffset; Z < HeightOffset + CHUNK_SIZE && Z <= Heights[HeighIndex]; Z++)
			{
				BuildBlockMesh(SectionID, {X, Y, Z});
			}
		}
	}
	Sections[SectionID].State = ESectionState::MESHED;
}

void AChunk::BuildBlockMesh(int SectionID, const FIntVector& LocalBlockPosition)
{
	const FBlock& Block = GetBlock(LocalBlockPosition);

	if (Block.State == EBlockState::GAS)
	{
		return;
	}

	// Liquid block
	if (Block.State == EBlockState::LIQUID)
	{
		for (auto& Direction : SixNeighborhoodDirections)
		{
			if (Direction == EMinecraftDirection::DOWN && LocalBlockPosition.Z == 0)
			{
				continue;
			}
			const FBlock& NeighborBlock = ChunkMap->GetNeighborBlock(Position, LocalBlockPosition, Direction);

			if (NeighborBlock.State == EBlockState::SOLID || NeighborBlock.State == EBlockState::LIQUID)
			{
				continue;
			}
			CreateBlockFace(LocalBlockPosition, Direction, Sections[SectionID].LiquidBlocksVertexBuffer,
			                Sections[SectionID].LiquidBlocksTriangleBuffer,
			                Sections[SectionID].LiquidBlocksUVBuffer, Sections[SectionID].LiquidBlocksVertexColorBuffer);
		}
		return;
	}

	// Solid block
	for (auto& Direction : SixNeighborhoodDirections)
	{
		if (Direction == EMinecraftDirection::DOWN && LocalBlockPosition.Z == 0)
		{
			continue;
		}
		const FBlock& NeighborBlock = ChunkMap->GetNeighborBlock(Position, LocalBlockPosition, Direction);

		if (NeighborBlock.State == EBlockState::SOLID)
		{
			continue;
		}
		CreateBlockFace(LocalBlockPosition, Direction, Sections[SectionID].SolidBlocksVertexBuffer, Sections[SectionID].SolidBlocksTriangleBuffer,
		                Sections[SectionID].SolidBlocksUVBuffer, Sections[SectionID].SolidBlocksVertexColorBuffer);
	}
}

void AChunk::CreateChunkBorder()
{
	FVector ChunkWorldPosition(Position.X * CHUNK_SIZE, Position.Y * CHUNK_SIZE, 0);

	// Vertices that represent the bottom face of the chunk
	FVector VertexA = FVector(ChunkWorldPosition.X, ChunkWorldPosition.Y, 0) * BLOCK_SIZE + FVector(0.1, 0.1, 0);
	FVector VertexB = FVector(ChunkWorldPosition.X, ChunkWorldPosition.Y + CHUNK_SIZE, 0) * BLOCK_SIZE + FVector(0.1, -0.1, 0);
	FVector VertexC = FVector(ChunkWorldPosition.X + CHUNK_SIZE, ChunkWorldPosition.Y + CHUNK_SIZE, 0) * BLOCK_SIZE + FVector(-0.1, -0.1, 0);
	FVector VertexD = FVector(ChunkWorldPosition.X + CHUNK_SIZE, ChunkWorldPosition.Y, 0) * BLOCK_SIZE + FVector(-0.1, 0.1, 0);

	// Vertices that represent the top face of the chunk
	FVector VertexE = FVector(ChunkWorldPosition.X, ChunkWorldPosition.Y, CHUNK_HEIGHT) * BLOCK_SIZE + FVector(0.1, 0.1, 0);
	FVector VertexF = FVector(ChunkWorldPosition.X, ChunkWorldPosition.Y + CHUNK_SIZE, CHUNK_HEIGHT) * BLOCK_SIZE + FVector(0.1, -0.1, 0);
	FVector VertexG = FVector(ChunkWorldPosition.X + CHUNK_SIZE, ChunkWorldPosition.Y + CHUNK_SIZE, CHUNK_HEIGHT) * BLOCK_SIZE +
		FVector(-0.1, -0.1, 0);
	FVector VertexH = FVector(ChunkWorldPosition.X + CHUNK_SIZE, ChunkWorldPosition.Y, CHUNK_HEIGHT) * BLOCK_SIZE + FVector(-0.1, 0.1, 0);

	ChunkBorderVertexBuffer.Append({VertexA, VertexB, VertexC, VertexD, VertexE, VertexF, VertexG, VertexH});

	// Front face
	ChunkBorderTriangleBuffer.Append({0, 1, 5});
	ChunkBorderTriangleBuffer.Append({0, 5, 4});

	// Right face
	ChunkBorderTriangleBuffer.Append({1, 2, 6});
	ChunkBorderTriangleBuffer.Append({1, 6, 5});

	// Back face
	ChunkBorderTriangleBuffer.Append({2, 3, 7});
	ChunkBorderTriangleBuffer.Append({2, 7, 6});

	// Left face
	ChunkBorderTriangleBuffer.Append({3, 0, 4});
	ChunkBorderTriangleBuffer.Append({3, 4, 7});
}

bool AChunk::IsGenerated() const
{
	return State == EChunkState::MESHED || State == EChunkState::DIRTY || State == EChunkState::GENERATED;
}

bool AChunk::IsMeshed() const
{
	return State == EChunkState::MESHED || State == EChunkState::DIRTY;
}

bool AChunk::IsDirty() const
{
	return State == EChunkState::DIRTY;
}

bool AChunk::IsEmpty() const
{
	return State == EChunkState::EMPTY;
}

bool AChunk::IsMeshVisible() const
{
	return SolidBlocksMesh->IsVisible() || LiquidBlocksMesh->IsVisible();
}

FVector AChunk::CreateVertex(const FIntVector& VertexCoordinates) const
{
	int32 Z_Coord = VertexCoordinates.Z;
	int32 Y_Coord = VertexCoordinates.Y + Position.Y * CHUNK_SIZE;
	int32 X_Coord = VertexCoordinates.X + Position.X * CHUNK_SIZE;

	return FVector(X_Coord, Y_Coord, Z_Coord) * BLOCK_SIZE;
}

void AChunk::CreateBlockFace(const FIntVector& BlockPosition, EMinecraftDirection Direction, TArray<FVector>& VertexBuffer,
                             TArray<int32>& TriangleBuffer, TArray<FVector2D>& UVBuffer, TArray<FColor>& ColorBuffer)
{
	EBlockFace Face = DirectionToBlockFace[Direction];
	FBlock& Block = GetBlock(BlockPosition);

	int32 a = VertexBuffer.Add(CreateVertex(BlockPosition + FIntVector(FBlock::FaceVertices[Face][0])));
	int32 b = VertexBuffer.Add(CreateVertex(BlockPosition + FIntVector(FBlock::FaceVertices[Face][1])));
	int32 c = VertexBuffer.Add(CreateVertex(BlockPosition + FIntVector(FBlock::FaceVertices[Face][2])));
	int32 d = VertexBuffer.Add(CreateVertex(BlockPosition + FIntVector(FBlock::FaceVertices[Face][3])));

	UVBuffer.Append(FBlock::FaceUVs);

	TArray<TArray<int32>> Indices = {{3, 7, 4, 6}, {5, 7, 4, 8}, {1, 5, 4, 2}, {1, 3, 4, 0}};

	for (auto& Neighborhood : Indices)
	{
		uint8 LightValue0 = ChunkMap->GetNeighborBlock(Position, BlockPosition, PlaneNeighborhood[Direction][Neighborhood[0]]).LightValue;
		uint8 LightValue1 = ChunkMap->GetNeighborBlock(Position, BlockPosition, PlaneNeighborhood[Direction][Neighborhood[1]]).LightValue;
		uint8 LightValue2 = ChunkMap->GetNeighborBlock(Position, BlockPosition, PlaneNeighborhood[Direction][Neighborhood[2]]).LightValue;
		uint8 LightValue3 = ChunkMap->GetNeighborBlock(Position, BlockPosition, PlaneNeighborhood[Direction][Neighborhood[3]]).LightValue;

		int32 Light = LightValue0 + LightValue1 + LightValue2;

		// Add light value of the block that's on a diagonal from the current block only if it isnt obscured by the non diagonal blocks
		Light += (LightValue0 == MIN_LIGHT_VALUE && LightValue1 == MIN_LIGHT_VALUE) ? MIN_LIGHT_VALUE : LightValue3;
		Light /= 4;

		ColorBuffer.Add({Block.ID, FBlock::FaceTextureIndex[Face], static_cast<uint8>(Light)});
	}
	TriangleBuffer.Append({a, b, c, a, c, d});
}

void AChunk::PropagateLightInChunk()
{
	TQueue<FIntVector> LightSources;
	const int32 MaxHeight = GetMaxHeight();

	for (int X = 0; X < CHUNK_SIZE; X++)
	{
		for (int Y = 0; Y < CHUNK_SIZE; Y++)
		{
			int32 HeightIndex = Y + X * CHUNK_SIZE;

			for (int Z = MaxHeight + 1; Z >= 0; Z--)
			{
				if (Z > Heights[HeightIndex])
				{
					LightSources.Enqueue({X, Y, Z});
				}
				else
				{
					GetBlock({X, Y, Z}).LightValue = MIN_LIGHT_VALUE;
				}
			}
		}
	}

	PropagateLightSourcesInChunk(LightSources);

	for (auto& Direction : FourNeighborhoodDirections)
	{
		LightPropagatedFromNeighbor[Direction] = false;
	}
}

void AChunk::BlendLightWithNeighborChunk(EMinecraftDirection Direction)
{
	AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, Direction);

	if (!NeighborChunk)
	{
		return;
	}
	if (LightPropagatedFromNeighbor[Direction] && NeighborChunk->LightPropagatedFromNeighbor[OppositEMinecraftDirection[Direction]])
	{
		return;
	}

	TQueue<FIntVector> ThisChunkLightSources;
	TQueue<FIntVector> NeighborChunkLightSources;

	const int32 ThisChunkMaxHeight = GetMaxHeight();
	const int32 NeighborChunkMaxHeight = NeighborChunk->GetMaxHeight();

	int32 Height = ThisChunkMaxHeight > NeighborChunkMaxHeight ? ThisChunkMaxHeight : NeighborChunkMaxHeight;

	if (Direction == EMinecraftDirection::EAST || Direction == EMinecraftDirection::WEST)
	{
		int32 Y = Direction == EMinecraftDirection::WEST ? 0 : CHUNK_SIZE - 1;

		for (int32 X = 0; X < CHUNK_SIZE; X++)
		{
			for (int32 Z = 0; Z <= Height + 1; Z++)
			{
				FIntVector ThisChunkBlockPosition(X, Y, Z);
				FIntVector NeighborChunkBlockPosition(X, CHUNK_SIZE - 1 - Y, Z);

				FBlock& ThisChunkBlock = GetBlock(ThisChunkBlockPosition);
				FBlock& NeighborChunkBlock = NeighborChunk->GetBlock(NeighborChunkBlockPosition);

				if (NeighborChunkBlock.State != EBlockState::SOLID && ThisChunkBlock.LightValue - 1 > NeighborChunkBlock.LightValue)
				{
					NeighborChunkBlock.LightValue = ThisChunkBlock.LightValue - 1;
					NeighborChunkLightSources.Enqueue(NeighborChunkBlockPosition);
				}
				if (ThisChunkBlock.State != EBlockState::SOLID && NeighborChunkBlock.LightValue - 1 > ThisChunkBlock.LightValue)
				{
					ThisChunkBlock.LightValue = NeighborChunkBlock.LightValue - 1;
					ThisChunkLightSources.Enqueue(ThisChunkBlockPosition);
				}
			}
		}
	}

	else if (Direction == EMinecraftDirection::SOUTH || Direction == EMinecraftDirection::NORTH)
	{
		int32 X = Direction == EMinecraftDirection::SOUTH ? 0 : CHUNK_SIZE - 1;
		for (int32 Y = 0; Y < CHUNK_SIZE; Y++)
		{
			for (int32 Z = 0; Z <= Height + 1; Z++)
			{
				FIntVector ThisChunkBlockPosition(X, Y, Z);
				FIntVector NeighborChunkBlockPosition(CHUNK_SIZE - 1 - X, Y, Z);

				FBlock& ThisChunkBlock = GetBlock(ThisChunkBlockPosition);
				FBlock& NeighborChunkBlock = NeighborChunk->GetBlock(NeighborChunkBlockPosition);

				if (NeighborChunkBlock.State != EBlockState::SOLID && ThisChunkBlock.LightValue - 1 > NeighborChunkBlock.LightValue)
				{
					NeighborChunkBlock.LightValue = ThisChunkBlock.LightValue - 1;
					NeighborChunkLightSources.Enqueue(NeighborChunkBlockPosition);
				}
				if (ThisChunkBlock.State != EBlockState::SOLID && NeighborChunkBlock.LightValue - 1 > ThisChunkBlock.LightValue)
				{
					ThisChunkBlock.LightValue = NeighborChunkBlock.LightValue - 1;
					ThisChunkLightSources.Enqueue(ThisChunkBlockPosition);
				}
			}
		}
	}

	PropagateLightSourcesInChunk(ThisChunkLightSources);
	NeighborChunk->PropagateLightSourcesInChunk(NeighborChunkLightSources);

	LightPropagatedFromNeighbor[Direction] = true;
	NeighborChunk->LightPropagatedFromNeighbor[OppositEMinecraftDirection[Direction]] = true;
}

void AChunk::PropagateLightFromNeighborChunks()
{
	if (AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, EMinecraftDirection::SOUTH))
	{
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::WEST);
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::EAST);
	}
	if (AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, EMinecraftDirection::NORTH))
	{
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::WEST);
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::EAST);
	}

	if (AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, EMinecraftDirection::EAST))
	{
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::SOUTH);
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::NORTH);
	}
	if (AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, EMinecraftDirection::WEST))
	{
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::SOUTH);
		NeighborChunk->BlendLightWithNeighborChunk(EMinecraftDirection::NORTH);
	}

	for (auto& Direction : FourNeighborhoodDirections)
	{
		BlendLightWithNeighborChunk(Direction);
	}
}

void AChunk::PropagateLightSourcesInChunk(TQueue<FIntVector>& LightSources)
{
	while (!LightSources.IsEmpty())
	{
		FIntVector LightSourcePosition;
		LightSources.Dequeue(LightSourcePosition);

		for (auto& Direction : SixNeighborhoodDirections)
		{
			FIntVector NeighborBlockPosition = LightSourcePosition + DirectionToVector3D[Direction];

			if (IsBlockOutOfChunkBounds(NeighborBlockPosition))
			{
				continue;
			}

			FBlock& Block = GetBlock(LightSourcePosition);
			FBlock& NeighborBlock = GetBlock(NeighborBlockPosition);

			if (NeighborBlock.State != EBlockState::SOLID && NeighborBlock.LightValue < Block.LightValue - 1)
			{
				NeighborBlock.LightValue = Block.LightValue - 1;
				LightSources.Enqueue(NeighborBlockPosition);
			}
		}
	}
}

void AChunk::SetChunkStateAsGenerated()
{
	PropagateLightInChunk();
	State = EChunkState::GENERATED;
	OnGenerated.ExecuteIfBound(this);
}

FIntVector AChunk::GetWorldPositionFromLocal(const FIntVector& LocalPosition) const
{
	int32 X_Coord = LocalPosition.X + Position.X * CHUNK_SIZE;
	int32 Y_Coord = LocalPosition.Y + Position.Y * CHUNK_SIZE;
	int32 Z_Coord = LocalPosition.Z;

	return FIntVector(X_Coord, Y_Coord, Z_Coord);
}

FIntPoint AChunk::GetPosition() const
{
	return Position;
}

bool AChunk::IsBlockOutOfChunkBounds(const FIntVector& BlockPosition) const
{
	return BlockPosition.X < 0 || BlockPosition.X >= CHUNK_SIZE ||
		BlockPosition.Y < 0 || BlockPosition.Y >= CHUNK_SIZE ||
		BlockPosition.Z < 0 || BlockPosition.Z >= CHUNK_HEIGHT;
}

void AChunk::SetBlock(const FIntVector& BlockPosition, const FBlock& Block)
{
	const int32 BlockIndex = BlockPosition.Y + (BlockPosition.X + BlockPosition.Z * CHUNK_SIZE) * CHUNK_SIZE;
	const int32 HeightIndex = BlockPosition.Y + BlockPosition.X * CHUNK_SIZE;

	Blocks[BlockIndex] = Block;

	if (Block.State != EBlockState::GAS)
	{
		int32 Height = Heights[HeightIndex];
		Height = BlockPosition.Z > Height ? BlockPosition.Z : Height;
		Heights[HeightIndex] = Height;
	}
	else
	{
		if (BlockPosition.Z == Heights[HeightIndex])
		{
			int32 Index = BlockIndex;
			int i;
			for (i = BlockPosition.Z; Index >= 0 && Blocks[Index].State == EBlockState::GAS; i--)
			{
				Index -= BLOCKS_PER_CHUNK_HEIGHT;
			}
			Heights[HeightIndex] = i;
		}
	}

	int32 SectionID = BlockPosition.Z / CHUNK_SIZE;
	Sections[SectionID].State = ESectionState::DIRTY;

	if (State == EChunkState::MESHED)
	{
		// Mark in-chunk neighbor sections dirty
		if (SectionID > 0 && Sections[SectionID - 1].State != ESectionState::EMPTY)
		{
			Sections[SectionID - 1].State = ESectionState::DIRTY;
		}
		if (SectionID + 1 < CHUNK_SECTIONS && Sections[SectionID + 1].State != ESectionState::EMPTY)
		{
			Sections[SectionID + 1].State = ESectionState::DIRTY;
		}

		// mark neighbor chunks sections dirty
		TArray<UE::Tasks::FTask> TaskArray;
		for (auto& Direction : EightNeighborhoodDirections)
		{
			AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, Direction);
			if (NeighborChunk)
			{
				if (NeighborChunk->Sections[SectionID].State != ESectionState::EMPTY)
				{
					NeighborChunk->Sections[SectionID].State = ESectionState::DIRTY;
				}

				if (SectionID > 0 && NeighborChunk->Sections[SectionID - 1].State != ESectionState::EMPTY)
				{
					NeighborChunk->Sections[SectionID - 1].State = ESectionState::DIRTY;
				}

				if (SectionID + 1 < CHUNK_SECTIONS && NeighborChunk->Sections[SectionID + 1].State != ESectionState::EMPTY)
				{
					NeighborChunk->Sections[SectionID + 1].State = ESectionState::DIRTY;
				}
				NeighborChunk->PropagateLightInChunk();
			}
		}

		PropagateLightInChunk();
		for (auto& Direction : EightNeighborhoodDirections)
		{
			if (AChunk* NeighborChunk = ChunkMap->GetNeighborChunk(this, Direction))
			{
				NeighborChunk->SetChunkDirty();
			}
		}
		SetChunkDirty();
	}
}

void AChunk::SetSectionDirty(int32 SectionID)
{
	Sections[SectionID].State = ESectionState::DIRTY;

	if (State == EChunkState::MESHED)
	{
		State = EChunkState::DIRTY;
		OnDirty.ExecuteIfBound(this);
	}
}

void AChunk::SetChunkDirty()
{
	if (State == EChunkState::MESHED)
	{
		State = EChunkState::DIRTY;
		OnDirty.ExecuteIfBound(this);
	}
}

int32 AChunk::GetMaxHeight() const
{
	int32 MaxHeight = -1;
	for (int i = 0; i < BLOCKS_PER_CHUNK_HEIGHT; i ++)
	{
		MaxHeight = Heights[i] > MaxHeight ? Heights[i] : MaxHeight;
	}
	return MaxHeight;
}

const FBlock& AChunk::GetBlock(const FIntVector& LocalBlockPosition) const
{
	if (IsBlockOutOfChunkBounds(LocalBlockPosition))
	{
		return BlockDatabase->GetEmptyBlock();
	}
	const int32 Index = LocalBlockPosition.Y + (LocalBlockPosition.X + LocalBlockPosition.Z * CHUNK_SIZE) * CHUNK_SIZE;
	return Blocks[Index];
}

FBlock& AChunk::GetBlock(const FIntVector& LocalBlockPosition)
{
	if (IsBlockOutOfChunkBounds(LocalBlockPosition))
	{
		return BlockDatabase->GetEmptyBlock();
	}
	const int32 Index = LocalBlockPosition.Y + (LocalBlockPosition.X + LocalBlockPosition.Z * CHUNK_SIZE) * CHUNK_SIZE;
	return Blocks[Index];
}

int32 AChunk::GetHeight(const FIntPoint& LocalBlockPosition) const
{
	return Heights[LocalBlockPosition.Y + LocalBlockPosition.X * CHUNK_SIZE];
}

FCriticalSection& AChunk::GetLock()
{
	return Lock;
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
}

void AChunk::RenderMesh()
{
	SolidBlocksMesh->CreateMeshSection(0, SolidBlocksVertexBuffer, SolidBlocksTriangleBuffer, {}, SolidBlocksUVBuffer, SolidBlocksVertexColorBuffer,
	                                   {}, true);
	LiquidBlocksMesh->CreateMeshSection(0, LiquidBlocksVertexBuffer, LiquidBlocksTriangleBuffer, {}, LiquidBlocksUVBuffer,
	                                    LiquidBlocksVertexColorBuffer, {}, false);
	ChunkBorderMesh->CreateMeshSection(0, ChunkBorderVertexBuffer, ChunkBorderTriangleBuffer, {}, {}, {}, {}, false);

	SolidBlocksVertexBuffer.Empty();
	SolidBlocksTriangleBuffer.Empty();
	SolidBlocksUVBuffer.Empty();
	SolidBlocksVertexColorBuffer.Empty();

	LiquidBlocksVertexBuffer.Empty();
	LiquidBlocksTriangleBuffer.Empty();
	LiquidBlocksUVBuffer.Empty();
	LiquidBlocksVertexColorBuffer.Empty();

	ShowMesh();
}
