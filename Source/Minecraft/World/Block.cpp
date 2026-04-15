// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"


const TMap<EBlockFace, TArray<FVector>> FBlock::FaceVertices = {
	{EBlockFace::FRONT, {{0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}}},
	{EBlockFace::BACK, {{1, 1, 0}, {1, 0, 0}, {1, 0, 1}, {1, 1, 1}}},
	{EBlockFace::RIGHT, {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}}},
	{EBlockFace::LEFT, {{1, 0, 0}, {0, 0, 0}, {0, 0, 1}, {1, 0, 1}}},
	{EBlockFace::UP, {{0, 0, 1}, {0, 1, 1}, {1, 1, 1}, {1, 0, 1}}},
	{EBlockFace::DOWN, {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0, 0, 0}}},
};

const TMap<EBlockFace, TArray<FVector>> FBlock::LiquidFaceVertices = {
	{EBlockFace::FRONT, {{0, 0, 0}, {0, 1, 0}, {0, 1, 0.9}, {0, 0, 0.9}}},
	{EBlockFace::BACK, {{1, 1, 0}, {1, 0, 0}, {1, 0, 0.9}, {1, 1, 0.9}}},
	{EBlockFace::RIGHT, {{0, 1, 0}, {1, 1, 0}, {1, 1, 0.9}, {0, 1, 0.9}}},
	{EBlockFace::LEFT, {{1, 0, 0}, {0, 0, 0}, {0, 0, 0.9}, {1, 0, 0.9}}},
	{EBlockFace::UP, {{0, 0, 0.9}, {0, 1, 0.9}, {1, 1, 0.9}, {1, 0, 0.9}}},
	{EBlockFace::DOWN, {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0, 0, 0}}},
};


const TArray<FVector2d> FBlock::FaceUVs =
{
	{{1, 1}, {0, 1}, {0, 0}, {1, 0}},
};

const TMap<EBlockFace, uint8> FBlock::FaceTextureIndex =
{
	{EBlockFace::FRONT, 1},
	{EBlockFace::BACK, 1},
	{EBlockFace::RIGHT, 1},
	{EBlockFace::LEFT, 1},
	{EBlockFace::UP, 0},
	{EBlockFace::DOWN, 2},
};
