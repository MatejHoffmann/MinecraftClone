// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "PauseMenuWidget.generated.h"

UCLASS()
class MINECRAFT_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/*
	 * Handle when player clicks on the controls button
	 */
	UFUNCTION()
	void HandleControlsButtonClick();
	/*
	 * Handle when player clicks on the Exit to main menu button
	 */
	UFUNCTION()
	void HandleExitToMainMenuButtonClick();

	/*
	 * Handle when player clicks on the Exit game button
	 */
	UFUNCTION()
	void HandleExitGameButton();

	/*
	 * Set Visibility of this widget
	 */
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> ControlsButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> ExitToMainMenuButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> ExitGameButton;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSubclassOf<UUserWidget> ControlsMenuClass;
	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSoftObjectPtr<UWorld> MainMenuLevel;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> ControlsMenu;
};
