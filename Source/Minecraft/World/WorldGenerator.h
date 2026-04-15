// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"
#include "Engine/DataTable.h"
#include "WorldGenerator.generated.h"


USTRUCT(BlueprintType)
struct FTerrainTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Continentalnes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Erosion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int PeaksAndValleys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MinHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxHeight;
};

UCLASS(Blueprintable)
class MINECRAFT_API UWorldGenerator : public UObject
{
	GENERATED_BODY()

public:
	virtual ~UWorldGenerator() override;

	/*
	 * Initialize the noise generator with the given seed
	 */
	void Init(int Seed = 1337);

	/*
	 * Get height of the terrain at the given X and Y coordinates
	 */
	int32 GetHeight(float X, float Y) const;

	/*
	 * Get Temperature at the given coordinates
	 */
	float GetTemperature(float X, float Y, float Z) const;

	/*
	 * Get Humidity at the given coordinates
	 */
	float GetHumidity(float X, float Y, float Z) const;

	/*
	 * Get Continentalness at the given X and Y coordinates (Cotinentalness determines how far from the see is the terrain)
	 */
	float GetContinentalness(float X, float Y) const;

	/*
	 * Get Erosion at the given X and Y coordinates (Erosion determines how high or low can the terrain be)
	 */
	float GetErosion(float X, float Y) const;

	/*
	 * Get Peaks or Valleys at the given X and Y coordinates (Peaks and Valleys determine how the terrain is shaped)
	 */
	float GetPeaksAndValleys(float X, float Y) const;

	/*
	 * Get Density at the given coordinates. (Determines whether the block is solid or not)
	 */
	float GetDensity(float X, float Y, float Z) const;

	int32 GetSurfaceLayerHeight(float X, float Y) const;
private:
	TPair<int32, int32> TerrainMap[6][6][5];

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> TerrainTable;

	FastNoiseLite* ContinentalnessGenerator;
	FastNoiseLite* ErosionGenerator;
	FastNoiseLite* PeaksAndValleysGenerator;
	FastNoiseLite* TemperatureGenerator;
	FastNoiseLite* HumidityGenerator;
	FastNoiseLite* DensityGenerator;
	FastNoiseLite* SurfaceLayerGenerator;
};
