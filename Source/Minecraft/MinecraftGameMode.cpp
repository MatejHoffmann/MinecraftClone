// Copyright Epic Games, Inc. All Rights Reserved.


#include "MinecraftGameMode.h"

void AMinecraftGameMode::StartPlay()
{
	Super::StartPlay();
	check(GEngine != nullptr);

	// Fog cvar
	IConsoleManager::Get().RegisterConsoleVariable(TEXT("mc.Fog"),
	                                               true,
	                                               TEXT("Turns the fog on or off"),
	                                               ECVF_Scalability | ECVF_RenderThreadSafe);

	// Chunk border cvar
	IConsoleManager::Get().RegisterConsoleVariable(TEXT("mc.ChunkBorder"),
	                                               false,
	                                               TEXT("Shows the chunk border"),
	                                               ECVF_Scalability | ECVF_RenderThreadSafe);

	// Chunk border cvar
	IConsoleManager::Get().RegisterConsoleVariable(TEXT("mc.DebugScreen"),
	                                               false,
	                                               TEXT("Shows the debug screen"),
	                                               ECVF_Scalability | ECVF_RenderThreadSafe);

	// Build tasks workers cvar
	IConsoleManager::Get().RegisterConsoleVariable(TEXT("mc.NBuildWorkers"),
												   3,
												   TEXT("Shows the debug screen"),
												   ECVF_Scalability | ECVF_RenderThreadSafe);

	// Meshing tasks workers cvar
	IConsoleManager::Get().RegisterConsoleVariable(TEXT("mc.NMeshingWorkers"),
												   2,
												   TEXT("Shows the debug screen"),
												   ECVF_Scalability | ECVF_RenderThreadSafe);
}
