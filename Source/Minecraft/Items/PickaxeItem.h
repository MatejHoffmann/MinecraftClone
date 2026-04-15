// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "PickaxeItem.generated.h"


UCLASS()
class MINECRAFT_API UPickaxeItem : public UItem
{
	GENERATED_BODY()

public:
	void Use();
};
