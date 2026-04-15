// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

static const uint8 CHUNK_SIZE = 16;
static const uint8 CHUNK_SECTIONS = 16;
static const int32 CHUNK_HEIGHT = CHUNK_SIZE * CHUNK_SECTIONS;
static const int32 BLOCKS_PER_CHUNK = CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE;
static const int32 BLOCKS_PER_CHUNK_HEIGHT = CHUNK_SIZE * CHUNK_SIZE;
static const int32 BLOCK_SIZE = 100;
static const uint8 MIN_RENDER_DISTANCE = 1;
static const uint8 MAX_RENDER_DISTANCE = 48;
static const uint8 DEFAULT_RENDER_DISTANCE = 16;
static const uint8 MIN_LIGHT_VALUE = 0;
static const uint8 MAX_LIGHT_VALUE = 15;

UENUM()
enum class EMinecraftDirection
{
	NONE,
	SOUTH,
	NORTH,
	WEST,
	EAST,

	SOUTH_EAST,
	SOUTH_WEST,
	NORTH_EAST,
	NORTH_WEST,

	UP,
	DOWN,

	SOUTH_UP,
	SOUTH_DOWN,
	NORTH_UP,
	NORTH_DOWN,
	WEST_UP,
	WEST_DOWN,
	EAST_UP,
	EAST_DOWN,
	SOUTH_EAST_UP,
	SOUTH_EAST_DOWN,
	SOUTH_WEST_UP,
	SOUTH_WEST_DOWN,
	NORTH_EAST_UP,
	NORTH_EAST_DOWN,
	NORTH_WEST_UP,
	NORTH_WEST_DOWN,
};

static const TMap<EMinecraftDirection, FIntVector> DirectionToVector3D =
{
	{EMinecraftDirection::NONE, {0, 0, 0}},
	{EMinecraftDirection::SOUTH, {-1, 0, 0}},
	{EMinecraftDirection::NORTH, {1, 0, 0}},
	{EMinecraftDirection::WEST, {0, -1, 0}},
	{EMinecraftDirection::EAST, {0, 1, 0}},
	{EMinecraftDirection::SOUTH_EAST, {-1, 1, 0}},
	{EMinecraftDirection::SOUTH_WEST, {-1, -1, 0}},
	{EMinecraftDirection::NORTH_EAST, {1, 1, 0}},
	{EMinecraftDirection::NORTH_WEST, {1, -1, 0}},
	{EMinecraftDirection::UP, {0, 0, 1}},
	{EMinecraftDirection::DOWN, {0, 0, -1}},
	{EMinecraftDirection::SOUTH_UP, {-1, 0, 1}},
	{EMinecraftDirection::SOUTH_DOWN, {-1, 0, -1}},
	{EMinecraftDirection::NORTH_UP, {1, 0, 1}},
	{EMinecraftDirection::NORTH_DOWN, {1, 0, -1}},
	{EMinecraftDirection::WEST_UP, {0, -1, 1}},
	{EMinecraftDirection::WEST_DOWN, {0, -1, -1}},
	{EMinecraftDirection::EAST_UP, {0, 1, 1}},
	{EMinecraftDirection::EAST_DOWN, {0, 1, -1}},
	{EMinecraftDirection::SOUTH_EAST_UP, {-1, 1, 1}},
	{EMinecraftDirection::SOUTH_EAST_DOWN, {-1, 1, -1}},
	{EMinecraftDirection::SOUTH_WEST_UP, {-1, -1, 1}},
	{EMinecraftDirection::SOUTH_WEST_DOWN, {-1, -1, -1}},
	{EMinecraftDirection::NORTH_EAST_UP, {1, 1, 1}},
	{EMinecraftDirection::NORTH_EAST_DOWN, {1, 1, -1}},
	{EMinecraftDirection::NORTH_WEST_UP, {1, -1, 1}},
	{EMinecraftDirection::NORTH_WEST_DOWN, {1, -1, -1}},
};

static const TMap<FIntVector, EMinecraftDirection> VectorToDirection3D =
{
	{{0, 0, 0}, EMinecraftDirection::NONE},
	{{-1, 0, 0}, EMinecraftDirection::SOUTH},
	{{1, 0, 0}, EMinecraftDirection::NORTH},
	{{0, -1, 0}, EMinecraftDirection::WEST},
	{{0, 1, 0}, EMinecraftDirection::EAST},
	{{-1, 1, 0}, EMinecraftDirection::SOUTH_EAST},
	{{-1, -1, 0}, EMinecraftDirection::SOUTH_WEST},
	{{1, 1, 0}, EMinecraftDirection::NORTH_EAST},
	{{1, -1, 0}, EMinecraftDirection::NORTH_WEST},
	{{0, 0, 1}, EMinecraftDirection::UP},
	{{0, 0, -1}, EMinecraftDirection::DOWN},
	{{-1, 0, 1}, EMinecraftDirection::SOUTH_UP},
	{{-1, 0, -1}, EMinecraftDirection::SOUTH_DOWN},
	{{1, 0, 1}, EMinecraftDirection::NORTH_UP},
	{{1, 0, -1}, EMinecraftDirection::NORTH_DOWN},
	{{0, -1, 1}, EMinecraftDirection::WEST_UP},
	{{0, -1, -1}, EMinecraftDirection::WEST_DOWN},
	{{0, 1, 1}, EMinecraftDirection::EAST_UP},
	{{0, 1, -1}, EMinecraftDirection::EAST_DOWN},
	{{-1, 1, 1}, EMinecraftDirection::SOUTH_EAST_UP},
	{{-1, 1, -1}, EMinecraftDirection::SOUTH_EAST_DOWN},
	{{-1, -1, 1}, EMinecraftDirection::SOUTH_WEST_UP},
	{{-1, -1, -1}, EMinecraftDirection::SOUTH_WEST_DOWN},
	{{1, 1, 1}, EMinecraftDirection::NORTH_EAST_UP},
	{{1, 1, -1}, EMinecraftDirection::NORTH_EAST_DOWN},
	{{1, -1, 1}, EMinecraftDirection::NORTH_WEST_UP},
	{{1, -1, -1}, EMinecraftDirection::NORTH_WEST_DOWN},
};


static const TMap<EMinecraftDirection, FIntPoint> DirectionToVector2D =
{
	{EMinecraftDirection::NONE, {0, 0}},
	{EMinecraftDirection::SOUTH, {-1, 0}},
	{EMinecraftDirection::NORTH, {1, 0}},
	{EMinecraftDirection::WEST, {0, -1}},
	{EMinecraftDirection::EAST, {0, 1}},
	{EMinecraftDirection::SOUTH_EAST, {-1, 1}},
	{EMinecraftDirection::SOUTH_WEST, {-1, -1}},
	{EMinecraftDirection::NORTH_EAST, {1, 1}},
	{EMinecraftDirection::NORTH_WEST, {1, -1}},
};

static const TMap<EMinecraftDirection, EMinecraftDirection> Direction3DTo2D =
{
	{EMinecraftDirection::SOUTH, EMinecraftDirection::SOUTH},
	{EMinecraftDirection::NORTH, EMinecraftDirection::NORTH},
	{EMinecraftDirection::WEST, EMinecraftDirection::WEST},
	{EMinecraftDirection::EAST, EMinecraftDirection::EAST},
	{EMinecraftDirection::SOUTH_EAST, EMinecraftDirection::SOUTH_EAST},
	{EMinecraftDirection::SOUTH_WEST, EMinecraftDirection::SOUTH_WEST},
	{EMinecraftDirection::NORTH_EAST, EMinecraftDirection::NORTH_EAST},
	{EMinecraftDirection::NORTH_WEST, EMinecraftDirection::NORTH_WEST},

	{EMinecraftDirection::UP, EMinecraftDirection::NONE},
	{EMinecraftDirection::DOWN, EMinecraftDirection::NONE},

	{EMinecraftDirection::SOUTH_UP, EMinecraftDirection::SOUTH},
	{EMinecraftDirection::SOUTH_DOWN, EMinecraftDirection::SOUTH},
	{EMinecraftDirection::NORTH_UP, EMinecraftDirection::NORTH},
	{EMinecraftDirection::NORTH_DOWN, EMinecraftDirection::NORTH},
	{EMinecraftDirection::WEST_UP, EMinecraftDirection::WEST},
	{EMinecraftDirection::WEST_DOWN, EMinecraftDirection::WEST},
	{EMinecraftDirection::EAST_UP, EMinecraftDirection::EAST},
	{EMinecraftDirection::EAST_DOWN, EMinecraftDirection::EAST},
	{EMinecraftDirection::SOUTH_EAST_UP, EMinecraftDirection::SOUTH_EAST},
	{EMinecraftDirection::SOUTH_EAST_DOWN, EMinecraftDirection::SOUTH_EAST},
	{EMinecraftDirection::SOUTH_WEST_UP, EMinecraftDirection::SOUTH_WEST},
	{EMinecraftDirection::SOUTH_WEST_DOWN, EMinecraftDirection::SOUTH_WEST},
	{EMinecraftDirection::NORTH_EAST_UP, EMinecraftDirection::NORTH_EAST},
	{EMinecraftDirection::NORTH_EAST_DOWN, EMinecraftDirection::NORTH_EAST},
	{EMinecraftDirection::NORTH_WEST_UP, EMinecraftDirection::NORTH_WEST},
	{EMinecraftDirection::NORTH_WEST_DOWN, EMinecraftDirection::NORTH_WEST},
};

static const TArray<EMinecraftDirection> EightNeighborhoodDirections =
{
	EMinecraftDirection::SOUTH,
	EMinecraftDirection::NORTH,
	EMinecraftDirection::WEST,
	EMinecraftDirection::EAST,
	EMinecraftDirection::SOUTH_EAST,
	EMinecraftDirection::SOUTH_WEST,
	EMinecraftDirection::NORTH_EAST,
	EMinecraftDirection::NORTH_WEST
};

static const TArray<EMinecraftDirection> FourNeighborhoodDirections =
{
	EMinecraftDirection::SOUTH,
	EMinecraftDirection::NORTH,
	EMinecraftDirection::WEST,
	EMinecraftDirection::EAST,
};

static const TArray<EMinecraftDirection> SixNeighborhoodDirections =
{
	EMinecraftDirection::SOUTH,
	EMinecraftDirection::NORTH,
	EMinecraftDirection::WEST,
	EMinecraftDirection::EAST,
	EMinecraftDirection::UP,
	EMinecraftDirection::DOWN
};


static const TMap<EMinecraftDirection, EMinecraftDirection> OppositEMinecraftDirection =
{
	{EMinecraftDirection::SOUTH, EMinecraftDirection::NORTH},
	{EMinecraftDirection::NORTH, EMinecraftDirection::SOUTH},
	{EMinecraftDirection::WEST, EMinecraftDirection::EAST},
	{EMinecraftDirection::EAST, EMinecraftDirection::WEST},

	{EMinecraftDirection::SOUTH_EAST, EMinecraftDirection::NORTH_WEST},
	{EMinecraftDirection::SOUTH_WEST, EMinecraftDirection::NORTH_EAST},
	{EMinecraftDirection::NORTH_EAST, EMinecraftDirection::SOUTH_WEST},
	{EMinecraftDirection::NORTH_WEST, EMinecraftDirection::SOUTH_EAST},

	{EMinecraftDirection::UP, EMinecraftDirection::DOWN},
	{EMinecraftDirection::DOWN, EMinecraftDirection::UP},

	{EMinecraftDirection::SOUTH_UP, EMinecraftDirection::NORTH_DOWN},
	{EMinecraftDirection::SOUTH_DOWN, EMinecraftDirection::NORTH_UP},
	{EMinecraftDirection::NORTH_UP, EMinecraftDirection::SOUTH_DOWN},
	{EMinecraftDirection::NORTH_DOWN, EMinecraftDirection::SOUTH_UP},
	{EMinecraftDirection::WEST_UP, EMinecraftDirection::EAST_DOWN},
	{EMinecraftDirection::WEST_DOWN, EMinecraftDirection::EAST_UP},
	{EMinecraftDirection::EAST_UP, EMinecraftDirection::WEST_DOWN},
	{EMinecraftDirection::EAST_DOWN, EMinecraftDirection::WEST_UP},
	{EMinecraftDirection::SOUTH_EAST_UP, EMinecraftDirection::NORTH_WEST_DOWN},
	{EMinecraftDirection::SOUTH_EAST_DOWN, EMinecraftDirection::NORTH_WEST_UP},
	{EMinecraftDirection::SOUTH_WEST_UP, EMinecraftDirection::NORTH_EAST_DOWN},
	{EMinecraftDirection::SOUTH_WEST_DOWN, EMinecraftDirection::NORTH_EAST_UP},
	{EMinecraftDirection::NORTH_EAST_UP, EMinecraftDirection::SOUTH_WEST_DOWN},
	{EMinecraftDirection::NORTH_EAST_DOWN, EMinecraftDirection::SOUTH_WEST_UP},
	{EMinecraftDirection::NORTH_WEST_UP, EMinecraftDirection::SOUTH_EAST_DOWN},
	{EMinecraftDirection::NORTH_WEST_DOWN, EMinecraftDirection::SOUTH_EAST_UP}
};

static const TMap<EMinecraftDirection, TArray<EMinecraftDirection>> PlaneNeighborhood =
{
	{
		EMinecraftDirection::SOUTH,
		{
			EMinecraftDirection::SOUTH_WEST_UP, EMinecraftDirection::SOUTH_UP, EMinecraftDirection::SOUTH_EAST_UP, EMinecraftDirection::SOUTH_WEST, EMinecraftDirection::SOUTH,
			EMinecraftDirection::SOUTH_EAST, EMinecraftDirection::SOUTH_WEST_DOWN, EMinecraftDirection::SOUTH_DOWN, EMinecraftDirection::SOUTH_EAST_DOWN
		}
	},
	{
		EMinecraftDirection::NORTH,
		{
			EMinecraftDirection::NORTH_EAST_UP, EMinecraftDirection::NORTH_UP, EMinecraftDirection::NORTH_WEST_UP, EMinecraftDirection::NORTH_EAST, EMinecraftDirection::NORTH,
			EMinecraftDirection::NORTH_WEST, EMinecraftDirection::NORTH_EAST_DOWN, EMinecraftDirection::NORTH_DOWN, EMinecraftDirection::NORTH_WEST_DOWN
		}
	},
	{
		EMinecraftDirection::WEST,
		{
			EMinecraftDirection::NORTH_WEST_UP, EMinecraftDirection::WEST_UP, EMinecraftDirection::SOUTH_WEST_UP, EMinecraftDirection::NORTH_WEST, EMinecraftDirection::WEST,
			EMinecraftDirection::SOUTH_WEST, EMinecraftDirection::NORTH_WEST_DOWN, EMinecraftDirection::WEST_DOWN, EMinecraftDirection::SOUTH_WEST_DOWN
		}
	},
	{
		EMinecraftDirection::EAST,
		{
			EMinecraftDirection::SOUTH_EAST_UP, EMinecraftDirection::EAST_UP, EMinecraftDirection::NORTH_EAST_UP, EMinecraftDirection::SOUTH_EAST, EMinecraftDirection::EAST,
			EMinecraftDirection::NORTH_EAST, EMinecraftDirection::SOUTH_EAST_DOWN, EMinecraftDirection::EAST_DOWN, EMinecraftDirection::NORTH_EAST_DOWN
		}
	},
	{
		EMinecraftDirection::UP,
		{
			EMinecraftDirection::NORTH_WEST_UP, EMinecraftDirection::NORTH_UP, EMinecraftDirection::NORTH_EAST_UP, EMinecraftDirection::WEST_UP, EMinecraftDirection::UP, EMinecraftDirection::EAST_UP,
			EMinecraftDirection::SOUTH_WEST_UP, EMinecraftDirection::SOUTH_UP, EMinecraftDirection::SOUTH_EAST_UP
		}
	},
	{
		EMinecraftDirection::DOWN,
		{
			EMinecraftDirection::SOUTH_WEST_DOWN, EMinecraftDirection::SOUTH_DOWN, EMinecraftDirection::SOUTH_EAST_DOWN, EMinecraftDirection::WEST_DOWN, EMinecraftDirection::DOWN,
			EMinecraftDirection::EAST_DOWN, EMinecraftDirection::NORTH_WEST_DOWN, EMinecraftDirection::NORTH_DOWN, EMinecraftDirection::NORTH_EAST_DOWN
		}
	}
};
