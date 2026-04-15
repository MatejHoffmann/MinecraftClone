// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionBarBoxWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
#include "Minecraft/Character/InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "ActionBarWidget.generated.h"

UCLASS()
class MINECRAFT_API UActionBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UActionBarWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	/*
	 * Set given action bar slot as active
	 */
	UFUNCTION(BlueprintCallable)
	void SetActive(int32 Index);

	/*
	 * Handle when the item in the action bar changes
	 */
	UFUNCTION()
	void HandleActionBarItemChanged(UItem* Item, int32 SlotIndex);

	/*
	 * Handle when the active item is changed
	 */
	UFUNCTION()
	void HandleActiveItemChanged(int SlotIndex);

private:
	int32 ActiveItem;

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> ItemsBoxGrid;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox0;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox1;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox2;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox3;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox4;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox5;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox6;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox7;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> ItemBox8;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UActionBarBoxWidget> HighlightedItemBox;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TArray<UActionBarBoxWidget*> ItemBoxes;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;
};
