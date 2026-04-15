// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minecraft/World/Block.h"
#include "Item.h"
#include "BlockItem.generated.h"


UCLASS()
class MINECRAFT_API UBlockItem : public UItem
{
	GENERATED_BODY()

public:
	/*
	 * Use the given item
	 */
	virtual void Use(const FVector& LookAtPosition, const FVector& LookFromPosition);

public:
	UPROPERTY(BlueprintReadWrite)
	EBlockType Type;
};
