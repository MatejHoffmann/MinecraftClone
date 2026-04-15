// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DebugScreenWidget.generated.h"


UCLASS()
class MINECRAFT_API UDebugScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void SetPlayerPositionText(const FIntVector& Position);
	void SetBlockLightLevelText(uint8 Level);
	void SetBlockIDText(uint8 ID);

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> BlockPosition;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> BlockLightLevel;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> BlockID;
};
