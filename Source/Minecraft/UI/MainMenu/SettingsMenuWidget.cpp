// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuWidget.h"

#include "Kismet/ImportanceSamplingLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Minecraft/MinecraftGameInstance.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RenderDistanceSlider->SetMinValue(MIN_RENDER_DISTANCE);
	RenderDistanceSlider->SetMaxValue(MAX_RENDER_DISTANCE);
	RenderDistanceSlider->SetStepSize(1);
	RenderDistanceSlider->SetValue(DEFAULT_RENDER_DISTANCE);
	CurrentRenderDistance = DEFAULT_RENDER_DISTANCE;
	RenderDistanceNumber->SetText(UKismetTextLibrary::Conv_IntToText(CurrentRenderDistance));
	RenderDistanceSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleSliderValueChange);
	BackButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackButtonClick);
}

void USettingsMenuWidget::HandleSliderValueChange(float Value)
{
	RenderDistanceSlider->SetValue(static_cast<uint8>(Value));
	RenderDistanceNumber->SetText(UKismetTextLibrary::Conv_IntToText(static_cast<uint8>(Value)));
	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());
	GameInstance->SetRenderDistance(static_cast<uint8>(Value));
	CurrentRenderDistance = static_cast<uint8>(Value);
}

void USettingsMenuWidget::HandleBackButtonClick()
{
	SetVisibility(ESlateVisibility::Hidden);
}
