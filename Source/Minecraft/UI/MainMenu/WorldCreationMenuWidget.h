// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "WorldCreationMenuWidget.generated.h"

UCLASS()
class MINECRAFT_API UWorldCreationMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/*
	 * Handle when player clicks on the back button
	 */
	UFUNCTION()
	void HandleBackButtonClick();

	/*
	 * Handle when player clicks on the create new world button
	 */
	UFUNCTION()
	void HandleCreateWorldButtonClick();
	/*
	 * Validate the world seed input
	 */
	UFUNCTION()
	void ValidateSeedInput(const FText& Text);

	/*
	 * Validate the world name input
	 */
	UFUNCTION()
	void ValidateNameInput(const FText& Text);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEditableTextBox> WorldNameInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEditableTextBox> WorldSeedInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UButton> CreateWorldButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UButton> BackButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> NameInputErrorText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> SeedInputErrorText;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSoftObjectPtr<UWorld> LevelToOpen;

private:
	bool bIsSeedValid;
	bool bIsNameValid;
};
