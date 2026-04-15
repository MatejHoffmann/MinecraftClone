// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

template <typename T, int32 N, int32 N1, int32 N2>
class MINECRAFT_API TFlatArray3D
{
public:
	TFlatArray3D();
	TFlatArray3D(const T& Value);
	void Init(const T& Value);
	T& operator()(const FIntVector& Position);
	T& operator()(int32 X, int32 Y, int32 Z);
	const T& operator()(const FIntVector& Position) const;
	const T& operator()(int32 X, int32 Y, int32 Z) const;

private:
	UPROPERTY()
	TArray<T> Values;
	int32 SizeX;
	int32 SizeY;
	int32 SizeZ;
};

template <typename T, int32 X, int32 Y, int32 Z>
TFlatArray3D<T, X, Y, Z>::TFlatArray3D() : SizeX(X), SizeY(Y), SizeZ(Z)
{
	Values.SetNumUninitialized(SizeX * SizeY * SizeZ);
}

template <typename T, int32 N, int32 N1, int32 N2>
TFlatArray3D<T, N, N1, N2>::TFlatArray3D(const T& Value): SizeX(X), SizeY(Y), SizeZ(Z)
{
	Values.Init(Value, SizeX * SizeY * SizeZ);
}

template <typename T, int32 N, int32 N1, int32 N2>
void TFlatArray3D<T, N, N1, N2>::Init(const T& Value)
{
	for (int x = 0; x < SizeX; x++)
	{
		for (int y = 0; y < SizeY; y++)
		{
			for (int z = 0; z < SizeZ; z++)
			{
				(*this)(x, y, z) = Value;
			}
		}
	}
}

template <typename T, int32 N, int32 N1, int32 N2>
T& TFlatArray3D<T, N, N1, N2>::operator()(const FIntVector& Position)
{
	return Values[(Position.X + (Position.Y + Position.Z * SizeY) * SizeX)];
}

template <typename T, int32 N, int32 N1, int32 N2>
T& TFlatArray3D<T, N, N1, N2>::operator()(int32 X, int32 Y, int32 Z)
{
	return Values[(X + (Y + Z * SizeY) * SizeX)];
}

template <typename T, int32 N, int32 N1, int32 N2>
const T& TFlatArray3D<T, N, N1, N2>::operator()(const FIntVector& Position) const
{
	return Values[(Position.X + (Position.Y + Position.Z * SizeY) * SizeX)];
}

template <typename T, int32 N, int32 N1, int32 N2>
const T& TFlatArray3D<T, N, N1, N2>::operator()(int32 X, int32 Y, int32 Z) const
{
	return Values[(X + (Y + Z * SizeY) * SizeX)];
}
