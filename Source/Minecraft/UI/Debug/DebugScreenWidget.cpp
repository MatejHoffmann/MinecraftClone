// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugScreenWidget.h"

#include "Kismet/KismetTextLibrary.h"


void UDebugScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDebugScreenWidget::SetPlayerPositionText(const FIntVector& Position)
{
	BlockPosition->SetText(UKismetTextLibrary::Conv_StringToText(Position.ToString()));
}

void UDebugScreenWidget::SetBlockLightLevelText(uint8 Level)
{
	BlockLightLevel->SetText(UKismetTextLibrary::Conv_IntToText(Level));
}

void UDebugScreenWidget::SetBlockIDText(uint8 ID)
{
	BlockID->SetText(UKismetTextLibrary::Conv_IntToText(ID));
}
