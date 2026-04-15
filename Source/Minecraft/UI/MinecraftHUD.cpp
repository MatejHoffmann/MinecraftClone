// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftHUD.h"

void AMinecraftHUD::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	DebugScreen = CreateWidget<UDebugScreenWidget>(Controller, DebugScreenClass);
	DebugScreen->AddToViewport();
	DebugScreen->SetVisibility(ESlateVisibility::Hidden);
}

void AMinecraftHUD::ShowPauseMenu()
{
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if (!PauseMenu)
	{
		PauseMenu = CreateWidget<UPauseMenuWidget>(Controller, PauseMenuClass);
		PauseMenu->AddToViewport();
	}
	PauseMenu->SetVisibility(ESlateVisibility::Visible);
	Controller->SetInputMode(FInputModeGameAndUI());
	Controller->SetShowMouseCursor(true);
}

void AMinecraftHUD::HidePauseMenu()
{
	if (!PauseMenu)
	{
		return;
	}
	PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	Controller->SetInputMode(FInputModeGameOnly());
	Controller->SetShowMouseCursor(false);
}

void AMinecraftHUD::ShowCrossHair()
{
	if (!CrossHair)
	{
		APlayerController* Controller = Cast<APlayerController>(GetOwner());
		CrossHair = CreateWidget<UUserWidget>(Controller, CrossHairClass);
		CrossHair->AddToViewport();
	}
	CrossHair->SetVisibility(ESlateVisibility::Visible);
}

void AMinecraftHUD::HideCrossHair()
{
	if (!CrossHair)
	{
		return;
	}
	CrossHair->SetVisibility(ESlateVisibility::Hidden);
}

void AMinecraftHUD::ShowActionBar()
{
	if (!ActionBar)
	{
		APlayerController* Controller = Cast<APlayerController>(GetOwner());
		ActionBar = CreateWidget<UActionBarWidget>(Controller, ActionBarClass);
		ActionBar->AddToViewport();
	}
	ActionBar->SetVisibility(ESlateVisibility::Visible);
}

void AMinecraftHUD::HideActionBar()
{
	if (!ActionBar)
	{
		return;
	}
	ActionBar->SetVisibility(ESlateVisibility::Hidden);
}

void AMinecraftHUD::ToggleDebugScreen()
{
	if (DebugScreen->IsVisible())
	{
		DebugScreen->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		DebugScreen->SetVisibility(ESlateVisibility::Visible);
	}
}

UDebugScreenWidget* AMinecraftHUD::GetDebugScreen()
{
	return DebugScreen;
}
