// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldCreationMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Minecraft/MinecraftGameInstance.h"

void UWorldCreationMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BackButton->OnClicked.AddUniqueDynamic(this, &UWorldCreationMenuWidget::HandleBackButtonClick);
	CreateWorldButton->OnClicked.AddUniqueDynamic(this, &UWorldCreationMenuWidget::HandleCreateWorldButtonClick);
	WorldSeedInput->OnTextChanged.AddUniqueDynamic(this, &UWorldCreationMenuWidget::ValidateSeedInput);
	WorldNameInput->OnTextChanged.AddUniqueDynamic(this, &UWorldCreationMenuWidget::ValidateNameInput);

	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());

	WorldNameInput->SetText(FText::FromString(GameInstance->GetAvailableSaveGameFileName()));
	WorldSeedInput->SetText(FText::FromString(FString::FromInt(FMath::Rand())));

	bIsSeedValid = true;
	bIsNameValid = true;
}

void UWorldCreationMenuWidget::HandleBackButtonClick()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UWorldCreationMenuWidget::HandleCreateWorldButtonClick()
{
	if (!bIsSeedValid || !bIsNameValid)
	{
		return;
	}

	FString WorldName = WorldNameInput->GetText().ToString();
	int32 WorldSeed = UKismetStringLibrary::Conv_StringToInt(WorldSeedInput->GetText().ToString());

	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());
	GameInstance->SetGameData(WorldName, WorldSeed, true);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LevelToOpen);
}

void UWorldCreationMenuWidget::ValidateSeedInput(const FText& Text)
{
	FString SeedInput = Text.ToString();

	if (SeedInput.IsEmpty())
	{
		bIsSeedValid = false;
		SeedInputErrorText->SetText(FText::FromString("Seed cannot be empty!"));
	}
	else if (!SeedInput.IsNumeric())
	{
		bIsSeedValid = false;
		SeedInputErrorText->SetText(FText::FromString("Seed is invalid! Must be numeric value!"));
	}
	else
	{
		bIsSeedValid = true;
		SeedInputErrorText->SetText(FText::FromString(""));
	}
}

void UWorldCreationMenuWidget::ValidateNameInput(const FText& Text)
{
	FString NameInput = Text.ToString();
	UMinecraftGameInstance* GameInstance = Cast<UMinecraftGameInstance>(GetGameInstance());

	if (NameInput.IsEmpty())
	{
		bIsNameValid = false;
		NameInputErrorText->SetText(FText::FromString("World name cannot be empty!"));
	}
	else if (!GameInstance->IsSaveGameFileNameAvailable(NameInput))
	{
		bIsNameValid = false;
		NameInputErrorText->SetText(FText::FromString("World name already exists!"));
	}
	else
	{
		bIsNameValid = true;
		NameInputErrorText->SetText(FText::FromString(""));
	}
}
