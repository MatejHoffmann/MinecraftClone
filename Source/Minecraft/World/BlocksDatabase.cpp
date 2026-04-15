// Fill out your copyright notice in the Description page of Project Settings.


#include "BlocksDatabase.h"
#include "Engine/Texture2DArray.h"

void UBlocksDatabase::Init()
{
	for (int i = 0; i < BlocksData->Blocks.Num(); i++)
	{
		BlocksMap.Add(BlocksData->Blocks[i].Type, i);
	}

	BlocksTopTexture = BlocksData->TopTextures;
	BlocksSideTexture = BlocksData->SidesTextures;
	BlocksBottomTexture = BlocksData->BottomTextures;

	SolidBlocksMaterialInstance = UMaterialInstanceDynamic::Create(SolidBlocksMaterial, this);
	SolidBlocksMaterialInstance->SetTextureParameterValue("TopTextures", BlocksTopTexture.Get());
	SolidBlocksMaterialInstance->SetTextureParameterValue("SideTextures", BlocksSideTexture.Get());
	SolidBlocksMaterialInstance->SetTextureParameterValue("BottomTextures", BlocksBottomTexture.Get());

	LiquidBlocksMaterialInstance = UMaterialInstanceDynamic::Create(LiquidBlocksMaterial, this);
	LiquidBlocksMaterialInstance->SetTextureParameterValue("TopTextures", BlocksTopTexture.Get());
	LiquidBlocksMaterialInstance->SetTextureParameterValue("SideTextures", BlocksSideTexture.Get());
	LiquidBlocksMaterialInstance->SetTextureParameterValue("BottomTextures", BlocksBottomTexture.Get());

	ChunkBorderMaterialInstance = UMaterialInstanceDynamic::Create(ChunkBorderMaterial, this);
	
	EmptyBlock = GetBlockByType(EBlockType::AIR);
}

UMaterialInstanceDynamic* UBlocksDatabase::GetSolidBlocksMaterial() const
{
	return SolidBlocksMaterialInstance;
}

UMaterialInstanceDynamic* UBlocksDatabase::GetLiquidBlocksMaterial() const
{
	return LiquidBlocksMaterialInstance;
}

UMaterialInstanceDynamic* UBlocksDatabase::GetChunkBorderMaterial() const
{
	return ChunkBorderMaterialInstance;
}

const FBlock& UBlocksDatabase::GetEmptyBlock() const
{
	return EmptyBlock;
}

FBlock& UBlocksDatabase::GetEmptyBlock()
{
	return EmptyBlock;
}

FBlock UBlocksDatabase::GetBlockByType(EBlockType Type) const
{
	FBlock Block;
	Block.LightValue = MAX_LIGHT_VALUE;
	Block.ID = BlocksMap[Type];
	Block.State = BlocksData->Blocks[BlocksMap[Type]].State;
	return Block;
}
