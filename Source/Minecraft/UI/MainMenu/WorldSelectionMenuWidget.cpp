// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSelectionMenuWidget.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Minecraft/MinecraftGameInstance.h"
#include "Components/ScrollBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Minecraft/Utils/FunctionLibrary.h"


void UWorldItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	WorldButton->OnClicked.AddDynamic(this, &UWorldItemWidget::HandleWorldButtonClick);
}

void UWorldItemWidget::SetWorldName(const FString Name)
{
	WorldName->SetText(FText::AsCultureInvariant(Name));
}

FString UWorldItemWidget::GetWorldName()
{
	return WorldName->GetText().ToString();
}

void UWorldItemWidget::HandleWorldButtonClick_Implementation()
{
	OnSelected.ExecuteIfBound(this);
}

void UWorldItemWidget::DeSelectItem_Implementation()
{
}

void UWorldSelectionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BackButton->OnClicked.AddUniqueDynamic(this, &UWorldSelectionMenuWidget::HandleBackButtonClick);
	DeleteSelectedWorldButton->OnClicked.AddUniqueDynamic(this, &UWorldSelectionMenuWidget::HandleDeleteWorldButtonClick);
	CreateNewWorldButton->OnClicked.AddUniqueDynamic(this, &UWorldSelectionMenuWidget::HandleCreateNewWorldButtonClick);
	PlaySelectedWorldButton->OnClicked.AddUniqueDynamic(this, &UWorldSelectionMenuWidget::HandlePlaySelectedWorldButtonClick);

	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());
	TArray<FString> AvailableWorldSaveFiles = GameInstance->GetAvailableSaveGameFiles();

	for (auto& WorldSaveFile : AvailableWorldSaveFiles)
	{
		UWorldItemWidget* WorldItem = CreateWidget<UWorldItemWidget>(this, WorldItemClass);
		WorldItem->SetWorldName(WorldSaveFile);

		UPanelSlot* PanelSlot = ExistingWorlds->AddChild(WorldItem);
		UScrollBoxSlot* BoxSlot = Cast<UScrollBoxSlot>(PanelSlot);

		BoxSlot->SetHorizontalAlignment(HAlign_Center);
		WorldItem->OnSelected.BindUObject(this, &UWorldSelectionMenuWidget::HandleSelectedWorld);
	}
}

void UWorldSelectionMenuWidget::HandleSelectedWorld(UWorldItemWidget* World)
{
	if (SelectedWorld && World != SelectedWorld)
	{
		SelectedWorld->DeSelectItem();
	}
	SelectedWorld = World;
}

void UWorldSelectionMenuWidget::HandleBackButtonClick()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UWorldSelectionMenuWidget::HandleDeleteWorldButtonClick()
{
	if (!SelectedWorld)
	{
		return;
	}
	ExistingWorlds->RemoveChild(SelectedWorld);
	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());
	GameInstance->DeleteSaveGameFile(SelectedWorld->GetWorldName());
	SelectedWorld = nullptr;
}

void UWorldSelectionMenuWidget::HandleCreateNewWorldButtonClick()
{
	if (!WorldCreationMenu)
	{
		WorldCreationMenu = CreateWidget<UUserWidget>(this, WorldCreationMenuClass);
		Minecraft::AddSubmenuWidget(CanvasPanel, WorldCreationMenu);
	}
	WorldCreationMenu->SetVisibility(ESlateVisibility::Visible);
}

void UWorldSelectionMenuWidget::HandlePlaySelectedWorldButtonClick()
{
	if (!SelectedWorld)
	{
		return;
	}

	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());
	GameInstance->SetGameData(SelectedWorld->GetWorldName(), 0, false);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LevelToOpen);
}
