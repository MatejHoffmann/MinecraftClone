// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"

#include "Kismet/KismetMathLibrary.h"


UWorldGenerator::~UWorldGenerator()
{
	delete ContinentalnessGenerator;
	delete ErosionGenerator;
	delete PeaksAndValleysGenerator;
	delete TemperatureGenerator;
	delete HumidityGenerator;
	delete DensityGenerator;
	delete SurfaceLayerGenerator;
}

void UWorldGenerator::Init(int Seed)
{
	ContinentalnessGenerator = new FastNoiseLite(Seed * 2);
	ErosionGenerator = new FastNoiseLite(Seed * 3);
	PeaksAndValleysGenerator = new FastNoiseLite(Seed * 4);
	TemperatureGenerator = new FastNoiseLite(Seed * 5);
	HumidityGenerator = new FastNoiseLite(Seed * 6);
	DensityGenerator = new FastNoiseLite(Seed * 7);
	SurfaceLayerGenerator = new FastNoiseLite(Seed * 8);

	ContinentalnessGenerator->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	ContinentalnessGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
	ContinentalnessGenerator->SetFrequency(0.0004);
	ContinentalnessGenerator->SetFractalOctaves(4);
	ContinentalnessGenerator->SetFractalLacunarity(2);
	ContinentalnessGenerator->SetFractalGain(0.3);
	ContinentalnessGenerator->SetFractalWeightedStrength(0);

	ErosionGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	ErosionGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
	ErosionGenerator->SetFrequency(0.0010);
	ErosionGenerator->SetFractalOctaves(5);
	ErosionGenerator->SetFractalLacunarity(2);
	ErosionGenerator->SetFractalGain(0.4);
	ErosionGenerator->SetFractalWeightedStrength(0);

	PeaksAndValleysGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	PeaksAndValleysGenerator->SetFractalType(FastNoiseLite::FractalType_PingPong);
	PeaksAndValleysGenerator->SetFrequency(0.005);
	PeaksAndValleysGenerator->SetFractalOctaves(5);
	PeaksAndValleysGenerator->SetFractalLacunarity(1.2);
	PeaksAndValleysGenerator->SetFractalGain(0.5);
	PeaksAndValleysGenerator->SetFractalWeightedStrength(0);
	PeaksAndValleysGenerator->SetFractalPingPongStrength(1.8);

	TemperatureGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	TemperatureGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
	TemperatureGenerator->SetFrequency(0.010);
	TemperatureGenerator->SetFractalOctaves(5);
	TemperatureGenerator->SetFractalLacunarity(2);
	TemperatureGenerator->SetFractalGain(0.4);
	TemperatureGenerator->SetFractalWeightedStrength(0);

	HumidityGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	HumidityGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
	HumidityGenerator->SetFrequency(0.010);
	HumidityGenerator->SetFractalOctaves(5);
	HumidityGenerator->SetFractalLacunarity(2);
	HumidityGenerator->SetFractalGain(0.4);
	HumidityGenerator->SetFractalWeightedStrength(0);

	DensityGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	DensityGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
	DensityGenerator->SetFrequency(0.01);
	DensityGenerator->SetFractalOctaves(5);
	DensityGenerator->SetFractalLacunarity(2);
	DensityGenerator->SetFractalGain(0.4);
	DensityGenerator->SetFractalWeightedStrength(0);

	SurfaceLayerGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	SurfaceLayerGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
	SurfaceLayerGenerator->SetFrequency(0.1);
	SurfaceLayerGenerator->SetFractalOctaves(3);
	SurfaceLayerGenerator->SetFractalLacunarity(0);
	SurfaceLayerGenerator->SetFractalGain(0);
	SurfaceLayerGenerator->SetFractalWeightedStrength(0);
	
	TArray<FName> RowNames = TerrainTable->GetRowNames();
	FString ContextString;

	for (auto& Name : RowNames)
	{
		FTerrainTableRow* Row = TerrainTable->FindRow<FTerrainTableRow>(Name, ContextString);
		TerrainMap[Row->Continentalnes][Row->Erosion][Row->PeaksAndValleys] = {Row->MinHeight, Row->MaxHeight};
	}
}


int32 UWorldGenerator::GetHeight(float X, float Y) const
{
	// The generated noise is in the [-1,1] range, this scales in into [0,5]
	float PV = (PeaksAndValleysGenerator->GetNoise(X, Y) + 1) * 2.5;
	int PVIndex = PV;
	float PVFactor = PV - PVIndex;

	// The generated noise is in the [-1,1] range, this scales in into [0,6]
	float Erosion = (ErosionGenerator->GetNoise(X, Y) + 1) * 3;
	int ErosionIndex = Erosion;
	int PrevErosionIndex = FMath::Max(ErosionIndex - 1, 0);
	float ErosionFactor = Erosion - ErosionIndex;

	// The generated noise is in the [-1,1] range, this scales in into [0,6]
	float Cont = (ContinentalnessGenerator->GetNoise(X, Y) + 1) * 3;
	int ContIndex = Cont;
	int PrevContIndex = FMath::Max(ContIndex - 1, 0);
	float ContFactor = Cont - ContIndex;

	int PrevContPrevErosionHeight = FMath::Lerp(TerrainMap[PrevContIndex][PrevErosionIndex][PVIndex].Key,
	                                            TerrainMap[PrevContIndex][PrevErosionIndex][PVIndex].Value, PVFactor);
	int PrevContErosionHeight = FMath::Lerp(TerrainMap[PrevContIndex][ErosionIndex][PVIndex].Key,
	                                        TerrainMap[PrevContIndex][ErosionIndex][PVIndex].Value, PVFactor);

	int ContPrevErosionHeight = FMath::Lerp(TerrainMap[ContIndex][PrevErosionIndex][PVIndex].Key,
	                                        TerrainMap[ContIndex][PrevErosionIndex][PVIndex].Value, PVFactor);
	int ContErosionHeight = FMath::Lerp(TerrainMap[ContIndex][ErosionIndex][PVIndex].Key,
	                                    TerrainMap[ContIndex][ErosionIndex][PVIndex].Value, PVFactor);

	float ErosionLerpFactor = FMath::Min(FMath::Pow(2.0f, 3.0f * ErosionFactor) - 1, 1.0f);
	float ContLerpFactor = FMath::Min(FMath::Pow(2.0f, 6.0f * ContFactor) - 1, 1.0f);

	int PrevContHeight = FMath::Lerp(PrevContPrevErosionHeight, PrevContErosionHeight, ErosionLerpFactor);
	int ContHeight = FMath::Lerp(ContPrevErosionHeight, ContErosionHeight, ErosionLerpFactor);

	return FMath::Lerp(PrevContHeight, ContHeight, ContLerpFactor);
}

float UWorldGenerator::GetTemperature(float X, float Y, float Z) const
{
	return TemperatureGenerator->GetNoise(X, Y, Z);
}

float UWorldGenerator::GetContinentalness(float X, float Y) const
{
	// The generated noise is in the [-1,1] range, this scales in into [0,5]
	float PV = (PeaksAndValleysGenerator->GetNoise(X, Y) + 1) * 2.5;
	int PVIndex = PV;
	return PVIndex;
}

float UWorldGenerator::GetErosion(float X, float Y) const
{
	// The generated noise is in the [-1,1] range, this scales in into [0,6]
	float Erosion = (ErosionGenerator->GetNoise(X, Y) + 1) * 3;
	int ErosionIndex = Erosion;
	return ErosionIndex;
}

float UWorldGenerator::GetPeaksAndValleys(float X, float Y) const
{
	// The generated noise is in the [-1,1] range, this scales in into [0,6]
	float Cont = (ContinentalnessGenerator->GetNoise(X, Y) + 1) * 3;
	int ContIndex = Cont;
	return ContIndex;
}

float UWorldGenerator::GetDensity(float X, float Y, float Z) const
{
	return DensityGenerator->GetNoise(X, Y, Z);
}

int32 UWorldGenerator::GetSurfaceLayerHeight(float X, float Y) const
{
	// The generated noise is in the [-1,1] range, this scales in into [1,5]
	int32 LayerHeight = (SurfaceLayerGenerator->GetNoise(X, Y) + 1) * 2 + 1;
	return LayerHeight;
}
