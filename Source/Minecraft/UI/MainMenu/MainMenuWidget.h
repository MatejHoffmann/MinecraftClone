// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class MINECRAFT_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/*
	 * Handle when player clicks on the Exit game button
	 */
	UFUNCTION()
	void HandleExitGameButtonClick();

	/*
	 * Handle when player clicks on the play button
	 */
	UFUNCTION()
	void HandlePlayButtonClick();

	/*
	 * Handle when player clicks on the settings button
	 */
	UFUNCTION()
	void HandleSettingsButtonClick();

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> PlayButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> SettingsButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSubclassOf<UUserWidget> WorldSelectionMenuClass;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSubclassOf<UUserWidget> SettingsMenuClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UUserWidget> WorldSelectionMenu;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UUserWidget> SettingsMenu;
};
