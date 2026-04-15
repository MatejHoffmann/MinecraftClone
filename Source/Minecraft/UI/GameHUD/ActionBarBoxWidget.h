// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minecraft/Items/Item.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ActionBarBoxWidget.generated.h"

UCLASS()
class MINECRAFT_API UActionBarBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<UItem> Item;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UCanvasPanel> MainCanvas;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Count;

	int32 NItems;
};
