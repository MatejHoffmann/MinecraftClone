// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Debug/DebugScreenWidget.h"
#include "GameFramework/HUD.h"
#include "GameHUD/ActionBarWidget.h"
#include "GameHUD/PauseMenuWidget.h"
#include "MinecraftHUD.generated.h"


UCLASS()
class MINECRAFT_API AMinecraftHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void ShowPauseMenu();
	UFUNCTION(BlueprintCallable)
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable)
	void ShowCrossHair();
	UFUNCTION(BlueprintCallable)
	void HideCrossHair();

	UFUNCTION(BlueprintCallable)
	void ShowActionBar();
	UFUNCTION(BlueprintCallable)
	void HideActionBar();

	UFUNCTION(BlueprintCallable)
	void ToggleDebugScreen();

	UFUNCTION(BlueprintCallable)
	UDebugScreenWidget* GetDebugScreen();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPauseMenuWidget> PauseMenuClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> CrossHairClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UActionBarWidget> ActionBarClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDebugScreenWidget> DebugScreenClass;

	UPROPERTY()
	TObjectPtr<UPauseMenuWidget> PauseMenu;
	UPROPERTY()
	TObjectPtr<UUserWidget> CrossHair;
	UPROPERTY()
	TObjectPtr<UActionBarWidget> ActionBar;
	UPROPERTY()
	TObjectPtr<UDebugScreenWidget> DebugScreen;
};
