// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlsMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Minecraft/MinecraftGameMode.h"

void UControlsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RenderDistanceSlider->SetMinValue(MIN_RENDER_DISTANCE);
	RenderDistanceSlider->SetMaxValue(MAX_RENDER_DISTANCE);
	RenderDistanceSlider->SetStepSize(1);

	AMinecraftGameMode* GameMode = Cast<AMinecraftGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	CurrentRenderDistance = GameMode->MinecraftWorld->GetRenderDistance();
	RenderDistanceSlider->SetValue(CurrentRenderDistance);
	RenderDistanceNumber->SetText(UKismetTextLibrary::Conv_IntToText(CurrentRenderDistance));
	RenderDistanceSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleSliderValueChange);
	BackButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackButtonClick);
	bRenderDistanceChanged = false;
}

void UControlsMenuWidget::HandleSliderValueChange(float Value)
{
	RenderDistanceSlider->SetValue(static_cast<uint8>(Value));
	RenderDistanceNumber->SetText(UKismetTextLibrary::Conv_IntToText(static_cast<uint8>(Value)));
	bRenderDistanceChanged = true;
	CurrentRenderDistance = static_cast<uint8>(Value);
}

void UControlsMenuWidget::HandleBackButtonClick()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UControlsMenuWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	if (bRenderDistanceChanged)
	{
		AMinecraftGameMode* GameMode = Cast<AMinecraftGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		GameMode->MinecraftWorld->ChangeRenderDistance(CurrentRenderDistance);
		bRenderDistanceChanged = false;
	}
}
