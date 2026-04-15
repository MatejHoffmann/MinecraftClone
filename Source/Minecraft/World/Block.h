// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinecraftWorldSettings.h"
#include "Block.generated.h"

UENUM()
enum class EBlockState
{
	SOLID,
	LIQUID,
	GAS
};

UENUM()
enum class EBlockType : uint8
{
	AIR,
	GRASS,
	DIRT,
	STONE,
	SAND,
	WATER
};

UENUM()
enum class EBlockFace : uint8
{
	BEGIN = 1,
	FRONT = 1 << 0,
	BACK = 1 << 1,
	RIGHT = 1 << 2,
	LEFT = 1 << 3,
	UP = 1 << 4,
	DOWN = 1 << 5,
	END = 1 << 6,
};

static const TMap<EMinecraftDirection, EBlockFace> DirectionToBlockFace =
{
	{EMinecraftDirection::SOUTH, EBlockFace::FRONT},
	{EMinecraftDirection::NORTH, EBlockFace::BACK},
	{EMinecraftDirection::EAST, EBlockFace::RIGHT},
	{EMinecraftDirection::WEST, EBlockFace::LEFT},
	{EMinecraftDirection::UP, EBlockFace::UP},
	{EMinecraftDirection::DOWN, EBlockFace::DOWN},
};

inline EBlockFace& operator++(EBlockFace& Face)
{
	Face = static_cast<EBlockFace>(static_cast<uint8>(Face) << 1);
	return Face;
}

inline EBlockFace& operator++(EBlockFace& Face, int)
{
	Face = static_cast<EBlockFace>(static_cast<uint8>(Face) << 1);
	return Face;
}

USTRUCT(BlueprintType)
struct FFace
{
	GENERATED_BODY()
	TArray<FVector> VertexBuffer;
	TArray<int32> TriangleBuffer;
	TArray<FVector2D> UVBuffer;
	TArray<FColor> VertexColorBuffer;
};


USTRUCT(BlueprintType)
struct FBlock
{
	GENERATED_BODY()
	FBlock(): ID(0), LightValue(MAX_LIGHT_VALUE), State(EBlockState::GAS)
	{
	}

	uint8 ID;
	uint8 LightValue;
	EBlockState State;

	static const TMap<EBlockFace, TArray<FVector>> FaceVertices;
	static const TMap<EBlockFace, TArray<FVector>> LiquidFaceVertices;
	static const TArray<FVector2d> FaceUVs;
	static const TMap<EBlockFace, uint8> FaceTextureIndex;
};
