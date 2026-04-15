// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ControlsMenuWidget.generated.h"

UCLASS()
class MINECRAFT_API UControlsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/*
	 * Handle when the render distance slider value changes
	 */
	UFUNCTION()
	void HandleSliderValueChange(float Value);

	/*
	 * Handle when player clicks on the back button
	 */
	UFUNCTION()
	void HandleBackButtonClick();

	/*
	 * Set Visibility of this widget
	 */
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

protected:
	UPROPERTY(EditAnywhere)
	uint8 CurrentRenderDistance;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<USlider> RenderDistanceSlider;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> RenderDistanceNumber;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> BackButton;

	bool bRenderDistanceChanged;
};
