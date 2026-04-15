// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Item.generated.h"


UCLASS(Blueprintable)
class MINECRAFT_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UItem()
	{
	}

	virtual ~UItem() override
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	UStaticMesh* PickUpMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item", meta=(MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	bool Stackable;

	virtual void UseRightClick(const FVector& LookAtPosition, const FVector& LookFromPosition) PURE_VIRTUAL(UItem,);
	virtual void UseLeftClick(const FVector& LookAtPosition, const FVector& LookFromPosition) PURE_VIRTUAL(UItem,);
};
