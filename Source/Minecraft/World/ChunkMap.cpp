// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkMap.h"
#include "Kismet/KismetMathLibrary.h"
#include "Minecraft/MinecraftGameState.h"
#include "GenericPlatform/GenericPlatformProcess.h"


// Sets default values
AChunkMap::AChunkMap()
{
	PrimaryActorTick.bCanEverTick = true;
	bChunkBorderVisibility = false;
	bBuildQueueWorkerOn = false;
	bMeshingQueueWorkerOn = false;
}

void AChunkMap::BeginDestroy()
{
	Super::BeginDestroy();

	FScopeLock UpdateScopeLock(&UpdateLock);
	bBuildQueueWorkerOn = false;
	bMeshingQueueWorkerOn = false;

	UE::Tasks::Wait(BuildTasks);
	UE::Tasks::Wait(MeshingTasks);

	BuildTasks.Empty();
	MeshingTasks.Empty();
}

void AChunkMap::Init(uint8 MapRadius, const FIntPoint& MapCenter, AWorldBuilder* Builder, UBlocksDatabase* Database)
{
	Radius = MapRadius + 2;
	Center = MapCenter;
	WorldBuilder = Builder;
	BlocksDatabase = Database;
}

void AChunkMap::Spawn()
{
	for (int i = -Radius; i <= Radius; i++)
	{
		for (int j = -Radius; j <= Radius; j++)
		{
			FIntPoint LocalPosition = {i, j};
			int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt(i * i + j * j));
			if (Distance <= Radius)
			{
				CreateChunk(LocalPosition, LocalPosition + Center, Distance);
			}
		}
	}
}

void AChunkMap::ChangeCenter(const FIntPoint& NewCenter)
{
	if (bChunkBorderVisibility)
	{
		LocalChunkMap[{0,0}]->HideBorder();
		LocalChunkMap[{NewCenter - Center}]->ShowBorder();
	}
	UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, NewCenter]()
	{
		FScopeLock UpdateScopeLock(&UpdateLock);

		bBuildQueueWorkerOn = false;
		bMeshingQueueWorkerOn = false;

		UE::Tasks::Wait(BuildTasks);
		UE::Tasks::Wait(MeshingTasks);

		BuildTasks.Empty();
		MeshingTasks.Empty();

		FIntPoint Offset = NewCenter - Center;
		for (int i = -Radius; i <= Radius; i++)
		{
			for (int j = -Radius; j <= Radius; j++)
			{
				FIntPoint LocalPosition = {i, j};
				const int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt(LocalPosition.SizeSquared()));

				if (Distance > Radius)
				{
					continue;
				}

				const int32 NewDistance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt((LocalPosition + Offset).SizeSquared()));

				if (NewDistance > Radius)
				{
					ReuseChunk(WorldChunkMap[LocalPosition * (-1) + Center], LocalPosition, LocalPosition + NewCenter, Distance);
					continue;
				}

				LocalChunkMap[LocalPosition] = WorldChunkMap[LocalPosition + NewCenter];

				BuildQueue.ChangePriority(LocalChunkMap[LocalPosition]->GetName(), Distance);
				MeshingQueue.ChangePriority(LocalChunkMap[LocalPosition]->GetName(), Distance);
				RenderQueue.ChangePriority(LocalChunkMap[LocalPosition]->GetName(), Distance);


				if (Distance < Radius - 1 && !LocalChunkMap[LocalPosition]->IsMeshVisible() && LocalChunkMap[LocalPosition]->IsGenerated())
				{
					FScopeLock VisibilityQueueScopeLock(&VisibilityQueueLock);
					VisibilityQueue.Enqueue({LocalChunkMap[LocalPosition], true});
				}

				if (Distance >= Radius - 1)
				{
					FScopeLock VisibilityQueueScopeLock(&VisibilityQueueLock);
					VisibilityQueue.Enqueue({LocalChunkMap[LocalPosition], false});
				}
			}
		}
		Center = NewCenter;
	}, LowLevelTasks::ETaskPriority::BackgroundHigh);
}

void AChunkMap::ChangeRadius(uint8 NewRadius)
{
	FScopeLock UpdateScopeLock(&UpdateLock);

	bBuildQueueWorkerOn = false;
	bMeshingQueueWorkerOn = false;

	UE::Tasks::Wait(BuildTasks);
	UE::Tasks::Wait(MeshingTasks);

	BuildTasks.Empty();
	MeshingTasks.Empty();

	NewRadius += 2;

	if (NewRadius == Radius)
	{
		return;
	}

	if (NewRadius < Radius)
	{
		for (int i = -Radius; i <= Radius; i++)
		{
			for (int j = -Radius; j <= Radius; j++)
			{
				FIntPoint LocalPosition = {i, j};
				int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt(LocalPosition.SizeSquared()));
				if (Distance > Radius)
				{
					continue;
				}
				if (Distance > NewRadius)
				{
					DeleteChunk(LocalChunkMap[LocalPosition], LocalPosition, LocalPosition + Center);
					LocalChunkMap.Remove(LocalPosition);
				}
				if (Distance == NewRadius || Distance == NewRadius - 1)
				{
					LocalChunkMap[LocalPosition]->HideMesh();
				}
			}
		}
	}
	else
	{
		for (int i = -NewRadius; i <= NewRadius; i++)
		{
			for (int j = -NewRadius; j <= NewRadius; j++)
			{
				FIntPoint LocalPosition = {i, j};
				int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt(LocalPosition.SizeSquared()));
				if (Distance <= NewRadius && !LocalChunkMap.Contains(LocalPosition))
				{
					CreateChunk(LocalPosition, LocalPosition + Center, Distance);
				}
				if (Distance < NewRadius - 1)
				{
					LocalChunkMap[LocalPosition]->ShowMesh();
				}
			}
		}
	}

	Radius = NewRadius;
}

void AChunkMap::AddBlock(EBlockType Type, const FVector& WorldPosition, const FIntVector& PlayerBlockPosition)
{
	FIntVector BlockWorldPosition(WorldPosition.X, WorldPosition.Y, WorldPosition.Z);
	BlockWorldPosition /= BLOCK_SIZE;

	FIntPoint ChunkPosition((BlockWorldPosition.X) / CHUNK_SIZE, (BlockWorldPosition.Y) / CHUNK_SIZE);
	ChunkPosition.X += WorldPosition.X < 0 ? -1 : 0;
	ChunkPosition.Y += WorldPosition.Y < 0 ? -1 : 0;

	BlockWorldPosition.X += WorldPosition.X < 0 ? -1 : 0;
	BlockWorldPosition.Y += WorldPosition.Y < 0 ? -1 : 0;
	BlockWorldPosition.Z += WorldPosition.Z < 0 ? -1 : 0;

	if (PlayerBlockPosition == BlockWorldPosition || PlayerBlockPosition + FIntVector(0, 0, 1) == BlockWorldPosition)
	{
		return;
	}

	FIntVector BlockLocalPosition(BlockWorldPosition.X - CHUNK_SIZE * ChunkPosition.X, BlockWorldPosition.Y - CHUNK_SIZE * ChunkPosition.Y,
	                              BlockWorldPosition.Z);

	AMinecraftGameState* GameState = Cast<AMinecraftGameState>(GetWorld()->GetGameState());
	GameState->AlterBlock(ChunkPosition, BlockLocalPosition, Type);
	WorldChunkMap[ChunkPosition]->SetBlock(BlockLocalPosition, BlocksDatabase->GetBlockByType(Type));
}

void AChunkMap::RemoveBlock(const FVector& WorldPosition)
{
	FIntVector BlockWorldPosition(WorldPosition);
	BlockWorldPosition /= BLOCK_SIZE;

	FIntPoint ChunkPosition((BlockWorldPosition.X) / CHUNK_SIZE, (BlockWorldPosition.Y) / CHUNK_SIZE);
	ChunkPosition.X += WorldPosition.X < 0 ? -1 : 0;
	ChunkPosition.Y += WorldPosition.Y < 0 ? -1 : 0;

	BlockWorldPosition.X += WorldPosition.X < 0 ? -1 : 0;
	BlockWorldPosition.Y += WorldPosition.Y < 0 ? -1 : 0;
	BlockWorldPosition.Z += WorldPosition.Z < 0 ? -1 : 0;

	FIntVector BlockLocalPosition(BlockWorldPosition.X - CHUNK_SIZE * ChunkPosition.X, BlockWorldPosition.Y - CHUNK_SIZE * ChunkPosition.Y,
	                              BlockWorldPosition.Z);

	AMinecraftGameState* GameState = Cast<AMinecraftGameState>(GetWorld()->GetGameState());
	GameState->AlterBlock(ChunkPosition, BlockLocalPosition, EBlockType::AIR);
	WorldChunkMap[ChunkPosition]->SetBlock(BlockLocalPosition, BlocksDatabase->GetEmptyBlock());
}

const FBlock& AChunkMap::GetBlock(const FVector& BlockPosition)
{
	FIntVector BlockWorldPosition(BlockPosition);
	BlockWorldPosition /= BLOCK_SIZE;

	FIntPoint ChunkPosition((BlockWorldPosition.X) / CHUNK_SIZE, (BlockWorldPosition.Y) / CHUNK_SIZE);
	ChunkPosition.X += BlockPosition.X < 0 ? -1 : 0;
	ChunkPosition.Y += BlockPosition.Y < 0 ? -1 : 0;

	BlockWorldPosition.X += BlockPosition.X < 0 ? -1 : 0;
	BlockWorldPosition.Y += BlockPosition.Y < 0 ? -1 : 0;
	BlockWorldPosition.Z += BlockPosition.Z < 0 ? -1 : 0;

	FIntVector BlockLocalPosition(BlockWorldPosition.X - CHUNK_SIZE * ChunkPosition.X, BlockWorldPosition.Y - CHUNK_SIZE * ChunkPosition.Y,
	                              BlockWorldPosition.Z);

	return WorldChunkMap[ChunkPosition]->GetBlock(BlockLocalPosition);
}

// Called when the game starts or when spawned
void AChunkMap::BeginPlay()
{
	Super::BeginPlay();
}

void AChunkMap::HandleChunkDirty(AChunk* Chunk)
{
	int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt((Chunk->GetPosition() - Center).SizeSquared()));
	FScopeLock MeshingQueueScopeLock(&MeshingQueueLock);
	MeshingQueue.Add(Distance, Chunk->GetName(), Chunk);
}

void AChunkMap::CreateChunk(const FIntPoint& LocalChunkPosition, const FIntPoint& WorldChunkPosition, int32 Priority)
{
	// Spawn new chunk
	AChunk* Chunk = Cast<AChunk>(GetWorld()->SpawnActor(AChunk::StaticClass()));

	Chunk->Init(WorldChunkPosition, BlocksDatabase, this);

	// Bind Delegates
	Chunk->OnDirty.BindUObject(this, &AChunkMap::HandleChunkDirty);

	// Add to build queue
	BuildQueue.Add(Priority, Chunk->GetName(), Chunk);

	// Add to maps
	FScopeLock WorldChunkMapScopeLock(&WorldChunkMapLock);
	WorldChunkMap.Add(WorldChunkPosition, Chunk);

	FScopeLock LocalChunkMapScopeLock(&LocalChunkMapLock);
	LocalChunkMap.Add(LocalChunkPosition, Chunk);
	Chunk->HideBorder();
	/*if (bChunkBorderVisibility)
	{
		Chunk->ShowBorder();
	}
	else
	{
		Chunk->HideBorder();
	}*/
}

void AChunkMap::ReuseChunk(AChunk* Chunk, const FIntPoint& LocalChunkPosition, const FIntPoint& WorldChunkPosition, int32 Priority)
{
	BuildQueue.Remove(Chunk->GetName());
	MeshingQueue.Remove(Chunk->GetName());
	RenderQueue.Remove(Chunk->GetName());

	FIntPoint OldWorldChunkPosition = Chunk->GetPosition();
	Chunk->Restart();
	Chunk->Init(WorldChunkPosition, BlocksDatabase, this);

	// Bind Delegates
	Chunk->OnDirty.BindUObject(this, &AChunkMap::HandleChunkDirty);

	FScopeLock VisibilityQueueScopeLock(&DestroyMeshQueueLock);
	DestroyMeshQueue.Enqueue(Chunk);

	// Add to maps
	FScopeLock WorldChunkMapScopeLock(&WorldChunkMapLock);
	WorldChunkMap.Remove(OldWorldChunkPosition);
	WorldChunkMap.Add(WorldChunkPosition, Chunk);

	FScopeLock LocalChunkMapScopeLock(&LocalChunkMapLock);
	LocalChunkMap.Add(LocalChunkPosition, Chunk);

	// Add to build queue
	BuildQueue.Add(Priority, Chunk->GetName(), Chunk);
}

void AChunkMap::DeleteChunk(AChunk* Chunk, const FIntPoint& LocalChunkPosition, const FIntPoint& WorldChunkPosition)
{
	BuildQueue.Remove(Chunk->GetName());
	MeshingQueue.Remove(Chunk->GetName());
	RenderQueue.Remove(Chunk->GetName());

	FScopeLock WorldChunkMapScopeLock(&WorldChunkMapLock);
	WorldChunkMap.Remove(WorldChunkPosition);

	FScopeLock LocalChunkMapScopeLock(&LocalChunkMapLock);
	LocalChunkMap.Remove(LocalChunkPosition);

	FScopeLock VisibilityQueueScopeLock(&VisibilityQueueLock);
	VisibilityQueue.Enqueue({Chunk, false});

	Chunk->Destroy();
}


void AChunkMap::BuildTask(int32 N)
{
	UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, N]()
	{
		FScopeLock UpdateScopeLock(&UpdateLock);
		if (!bBuildQueueWorkerOn)
		{
			bBuildQueueWorkerOn = true;

			for (int i = 0; i < N; i++)
			{
				BuildTasks.Add(UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]()
				{
					while (!BuildQueue.IsEmpty() && bBuildQueueWorkerOn)
					{
						AChunk* Chunk;
						{
							FScopeLock BuildQueueScopeLock(&BuildQueueLock);
							if (!BuildQueue.Pop(Chunk))
							{
								break;
							}
						}

						if (!IsValid(Chunk))
						{
							continue;
						}

						Chunk = WorldBuilder->BuildChunk(Chunk);
						int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt((Chunk->GetPosition() - Center).SizeSquared()));

						FScopeLock MeshingQueueScopeLock(&MeshingQueueLock);
						MeshingQueue.Add(Distance, Chunk->GetName(), Chunk);
					}
					bBuildQueueWorkerOn = false;
				}, LowLevelTasks::ETaskPriority::BackgroundHigh));
			}
		}
	}, LowLevelTasks::ETaskPriority::BackgroundHigh);
}

void AChunkMap::MeshingTask(int32 N)
{
	UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, N]()
	{
		FScopeLock UpdateScopeLock(&UpdateLock);
		if (!bMeshingQueueWorkerOn)
		{
			bMeshingQueueWorkerOn = true;

			for (int i = 0; i < N; i++)
			{
				MeshingTasks.Add(UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]()
				{
					while (bMeshingQueueWorkerOn)
					{
						AChunk* Chunk;

						{
							FScopeLock MeshingQueueScopeLock(&MeshingQueueLock);
							if (MeshingQueue.GetMinPriority() >= Radius - 1 || !MeshingQueue.Pop(Chunk))
							{
								break;
							}
						}

						if (!IsValid(Chunk))
						{
							continue;
						}

						int32 Distance = UKismetMathLibrary::Round(UKismetMathLibrary::Sqrt((Chunk->GetPosition() - Center).SizeSquared()));

						if (!AreNeighborsReady(Chunk))
						{
							MeshingQueue.Add(Distance, Chunk->GetName(), Chunk);
							continue;
						}

						{
							FScopeLock MeshingNeighborsScopeLock(&MeshingNeighborsLock);
							Chunk->GetLock().Lock();
							for (auto& Direction : EightNeighborhoodDirections)
							{
								AChunk* NeighborChunk = GetNeighborChunk(Chunk, Direction);
								NeighborChunk->GetLock().Lock();
							}
						}

						Chunk->BuildMesh();

						Chunk->GetLock().Unlock();
						for (auto& Direction : EightNeighborhoodDirections)
						{
							AChunk* NeighborChunk = GetNeighborChunk(Chunk, Direction);
							NeighborChunk->GetLock().Unlock();
						}

						RenderQueue.Add(Distance, Chunk->GetName(), Chunk);
					}
					bMeshingQueueWorkerOn = false;
				}, LowLevelTasks::ETaskPriority::BackgroundHigh));
			}
		}
	}, LowLevelTasks::ETaskPriority::BackgroundHigh);
}

void AChunkMap::RenderTasks(int32 N)
{
	for (int i = 0; i < N; i++)
	{
		AChunk* Chunk;
		if (!RenderQueue.Pop(Chunk))
		{
			break;
		}
		Chunk->RenderMesh();
		Chunk->ShowMesh();
	}
}

bool AChunkMap::AreNeighborsReady(AChunk* Chunk)
{
	for (auto& Direction : EightNeighborhoodDirections)
	{
		AChunk* NeighborChunk = GetNeighborChunk(Chunk, Direction);
		if (!NeighborChunk || !NeighborChunk->IsGenerated())
		{
			return false;
		}
	}
	return true;
}

AChunk* AChunkMap::GetNeighborChunk(const FIntPoint& ChunkPosition, EMinecraftDirection Direction) const
{
	return GetChunk(ChunkPosition + DirectionToVector2D[Direction]);
}

AChunk* AChunkMap::GetNeighborChunk(const AChunk* Chunk, EMinecraftDirection Direction) const
{
	return GetChunk(Chunk->GetPosition() + DirectionToVector2D[Direction]);
}

AChunk* AChunkMap::GetChunk(const FIntPoint& ChunkPosition) const
{
	AChunk* const* NeighborChunk = WorldChunkMap.Find(ChunkPosition);

	if (NeighborChunk)
	{
		return *NeighborChunk;
	}

	return nullptr;
}

FBlock& AChunkMap::GetNeighborBlock(const FIntVector& BlockWorldPosition, EMinecraftDirection Direction) const
{
	return GetBlock(BlockWorldPosition + DirectionToVector3D[Direction]);
}

FBlock& AChunkMap::GetNeighborBlock(const FIntPoint& ChunkPosition, const FIntVector& LocalBlockPosition, EMinecraftDirection Direction) const
{
	FIntVector NeighborBlockLocalPosition = LocalBlockPosition + DirectionToVector3D[Direction];
	NeighborBlockLocalPosition.X = (NeighborBlockLocalPosition.X + CHUNK_SIZE) % CHUNK_SIZE;
	NeighborBlockLocalPosition.Y = (NeighborBlockLocalPosition.Y + CHUNK_SIZE) % CHUNK_SIZE;

	FIntPoint ChangeInChunkPosition;
	ChangeInChunkPosition.X = (FMath::Abs(NeighborBlockLocalPosition.X - LocalBlockPosition.X) > 1) * DirectionToVector2D[Direction3DTo2D[Direction]].
		X;
	ChangeInChunkPosition.Y = (FMath::Abs(NeighborBlockLocalPosition.Y - LocalBlockPosition.Y) > 1) * DirectionToVector2D[Direction3DTo2D[Direction]].
		Y;

	return GetBlock(ChunkPosition + ChangeInChunkPosition, NeighborBlockLocalPosition);
}

FBlock& AChunkMap::GetBlock(const FIntVector& WorldBlockPosition) const
{
	const FIntPoint ChunkPosition(FMath::Floor(WorldBlockPosition.X / CHUNK_SIZE), FMath::Floor(WorldBlockPosition.Y / CHUNK_SIZE));
	const FIntVector BlockLocalPosition(WorldBlockPosition.X - CHUNK_SIZE * ChunkPosition.X, WorldBlockPosition.Y - CHUNK_SIZE * ChunkPosition.Y,
	                                    WorldBlockPosition.Z);

	return GetBlock(ChunkPosition, BlockLocalPosition);
}

FBlock& AChunkMap::GetBlock(const FIntPoint& ChunkPosition, const FIntVector& BlockLocalPosition) const
{
	AChunk* Chunk = GetChunk(ChunkPosition);

	if (Chunk)
	{
		return Chunk->GetBlock(BlockLocalPosition);
	}
	return BlocksDatabase->GetEmptyBlock();
}

void AChunkMap::ToggleChunkBorderVisibility()
{
	bChunkBorderVisibility = !bChunkBorderVisibility;
	for (auto& PositionChunk : LocalChunkMap)
	{
		if (bChunkBorderVisibility)
		{
			PositionChunk.Value->ShowBorder();
		}
		else
		{
			PositionChunk.Value->HideBorder();
		}
	}
}

// Called every frame
void AChunkMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto CVarBuildWorkers = IConsoleManager::Get().FindConsoleVariable(TEXT("mc.NBuildWorkers"));
	int32 NBuildWorkers = CVarBuildWorkers->GetInt();

	const auto CVarMeshingWorkers = IConsoleManager::Get().FindConsoleVariable(TEXT("mc.NMeshingWorkers"));
	int32 NMeshingWorkers = CVarBuildWorkers->GetInt();

	
	BuildTask(NBuildWorkers);
	MeshingTask(NMeshingWorkers);
	RenderTasks(9);

	{
		FScopeLock VisibilityQueueScopeLock(&VisibilityQueueLock);
		while (!VisibilityQueue.IsEmpty())
		{
			TPair<AChunk*, bool> QueueValue;
			VisibilityQueue.Dequeue(QueueValue);
			if (QueueValue.Value)
			{
				QueueValue.Key->ShowMesh();
			}
			else
			{
				QueueValue.Key->HideMesh();
			}
		}
	}

	{
		FScopeLock DestroyMeshQueueScopeLock(&DestroyMeshQueueLock);
		while (!DestroyMeshQueue.IsEmpty())
		{
			AChunk* Chunk;
			DestroyMeshQueue.Dequeue(Chunk);
			Chunk->DestroyMesh();
		}
	}
	
	const auto CVarChunkBorder = IConsoleManager::Get().FindConsoleVariable(TEXT("mc.ChunkBorder"));
	bool Value = CVarChunkBorder->GetBool();
	
	if (!bChunkBorderVisibility && Value)
	{
		LocalChunkMap[{0,0}]->ShowBorder();
		/*for (auto& PositionChunk : LocalChunkMap)
		{
			PositionChunk.Value->ShowBorder();
		}*/
		bChunkBorderVisibility = true;
	}
	else if (bChunkBorderVisibility && !Value)
	{
		LocalChunkMap[{0,0}]->HideBorder();
		/*for (auto& PositionChunk : LocalChunkMap)
		{
			PositionChunk.Value->HideBorder();
		}*/
		bChunkBorderVisibility = false;
	}
}
