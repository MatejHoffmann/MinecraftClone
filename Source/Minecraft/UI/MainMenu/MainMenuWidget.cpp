// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Minecraft/Utils/FunctionLibrary.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ExitButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::HandleExitGameButtonClick);
	PlayButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::HandlePlayButtonClick);
	SettingsButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::HandleSettingsButtonClick);
}

void UMainMenuWidget::HandleExitGameButtonClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UMainMenuWidget::HandlePlayButtonClick()
{
	if (!WorldSelectionMenu)
	{
		WorldSelectionMenu = CreateWidget<UUserWidget>(this, WorldSelectionMenuClass);
		Minecraft::AddSubmenuWidget(CanvasPanel, WorldSelectionMenu);
	}
	WorldSelectionMenu->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::HandleSettingsButtonClick()
{
	if (!SettingsMenu)
	{
		SettingsMenu = CreateWidget<UUserWidget>(this, SettingsMenuClass);
		Minecraft::AddSubmenuWidget(CanvasPanel, SettingsMenu);
	}
	SettingsMenu->SetVisibility(ESlateVisibility::Visible);
}
