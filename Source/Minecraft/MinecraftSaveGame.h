// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "World/BlocksDatabase.h"
#include "GameFramework/SaveGame.h"
#include "MinecraftSaveGame.generated.h"


UCLASS()
class MINECRAFT_API UMinecraftSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FVector PlayerPosition;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FRotator PlayerRotation;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	int Seed;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FIntVector> AlteredBlocksBlockPosition;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FIntPoint> AlteredBlocksChunkPosition;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<EBlockType> AlteredBlocksBlockType;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
};
