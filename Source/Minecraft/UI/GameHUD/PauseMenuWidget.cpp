// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Minecraft/MinecraftGameMode.h"
#include "Minecraft/Utils/FunctionLibrary.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ControlsButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::HandleControlsButtonClick);
	ExitToMainMenuButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::HandleExitToMainMenuButtonClick);
	ExitGameButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::HandleExitGameButton);
}


void UPauseMenuWidget::HandleControlsButtonClick()
{
	if (!ControlsMenu)
	{
		ControlsMenu = CreateWidget<UUserWidget>(this, ControlsMenuClass);
		Minecraft::AddSubmenuWidget(CanvasPanel, ControlsMenu);
	}
	ControlsMenu->SetVisibility(ESlateVisibility::Visible);
}

void UPauseMenuWidget::HandleExitToMainMenuButtonClick()
{
	AMinecraftGameMode* GameMode = Cast<AMinecraftGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->MinecraftWorld->SaveWorld();
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MainMenuLevel);
}

void UPauseMenuWidget::HandleExitGameButton()
{
	AMinecraftGameMode* GameMode = Cast<AMinecraftGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->MinecraftWorld->SaveWorld();
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UPauseMenuWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	if (ControlsMenu)
	{
		ControlsMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}
