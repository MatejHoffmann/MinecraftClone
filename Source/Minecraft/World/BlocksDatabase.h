// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Object.h"
#include "Block.h"
#include "BlocksDatabase.generated.h"


/*
 * Struct for the block data asset
 */
USTRUCT(BlueprintType)
struct FBlockData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBlockType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBlockState State;
};

UCLASS()
class MINECRAFT_API UBlocksDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FBlockData> Blocks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2DArray> TopTextures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2DArray> SidesTextures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2DArray> BottomTextures;
};

UCLASS(Blueprintable)
class MINECRAFT_API UBlocksDatabase : public UObject
{
	GENERATED_BODY()

public:
	/*
	 * Initialize the database and loads all data from the editor
	 */
	UFUNCTION(BlueprintCallable)
	void Init();

	/*
	 * Creates new block of the given type
	 */
	UFUNCTION(BlueprintCallable)
	FBlock GetBlockByType(EBlockType Type) const;

	/*
	 * Get material instance of the solid blocks
	 */
	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* GetSolidBlocksMaterial() const;

	/*
	 * Get material instance of the liquid blocks
	 */
	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* GetLiquidBlocksMaterial() const;

	/*
	 * Get material instance of the the border material
	 */
	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* GetChunkBorderMaterial() const;

	/*
	 * Returns an empty block
	 */
	const FBlock& GetEmptyBlock() const;
	FBlock& GetEmptyBlock();

private:
	// Blocks Data
	UPROPERTY(EditDefaultsOnly, Category="Blocks Data")
	TObjectPtr<UBlocksDataAsset> BlocksData;
	UPROPERTY()
	TMap<EBlockType, int32> BlocksMap;

	// Textures
	UPROPERTY()
	TObjectPtr<UTexture2DArray> BlocksTopTexture;
	UPROPERTY()
	TObjectPtr<UTexture2DArray> BlocksSideTexture;
	UPROPERTY()
	TObjectPtr<UTexture2DArray> BlocksBottomTexture;

	// Materials
	UPROPERTY(EditDefaultsOnly, Category="Minecraft|Materials")
	TObjectPtr<UMaterial> SolidBlocksMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Minecraft|Materials")
	TObjectPtr<UMaterial> LiquidBlocksMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SolidBlocksMaterialInstance;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> LiquidBlocksMaterialInstance;

	UPROPERTY(EditDefaultsOnly, Category="Minecraft|Materials")
	TObjectPtr<UMaterial> ChunkBorderMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ChunkBorderMaterialInstance;

	FBlock EmptyBlock;
};
