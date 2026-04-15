// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBarWidget.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/KismetTextLibrary.h"
#include "Minecraft/Character/MinecraftCharacter.h"

UActionBarWidget::UActionBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UActionBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActiveItem = 0;
	ItemBoxes.Add(ItemBox0);
	ItemBoxes.Add(ItemBox1);
	ItemBoxes.Add(ItemBox2);
	ItemBoxes.Add(ItemBox3);
	ItemBoxes.Add(ItemBox4);
	ItemBoxes.Add(ItemBox5);
	ItemBoxes.Add(ItemBox6);
	ItemBoxes.Add(ItemBox7);
	ItemBoxes.Add(ItemBox8);

	SetActive(0);

	APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	AMinecraftCharacter* Player = Cast<AMinecraftCharacter>(Pawn);
	Inventory = Player->InventoryComponent;
	Inventory->OnActionBarItemChange.BindUObject(this, &UActionBarWidget::HandleActionBarItemChanged);
	Inventory->OnActiveSlotChange.BindUObject(this, &UActionBarWidget::HandleActiveItemChanged);

	for (auto& ItemBox : ItemBoxes)
	{
		ItemBox->Count->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UActionBarWidget::HandleActionBarItemChanged(UItem* Item, int32 SlotIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player Location: %S"), "Image Set"));

	ItemBoxes[SlotIndex]->Image->SetBrushFromTexture(Item->Thumbnail);
	ItemBoxes[SlotIndex]->Image->SetVisibility(ESlateVisibility::Visible);
	ItemBoxes[SlotIndex]->Item = Item;
	ItemBoxes[SlotIndex]->Count->SetVisibility(ESlateVisibility::Visible);
	ItemBoxes[SlotIndex]->Count->SetText(UKismetTextLibrary::Conv_IntToText(1));
	SetActive(ActiveItem);
}

void UActionBarWidget::HandleActiveItemChanged(int SlotIndex)
{
	SetActive(SlotIndex);
}

void UActionBarWidget::SetActive(int32 index)
{
	ItemBoxes[ActiveItem]->MainCanvas->SetVisibility(ESlateVisibility::Visible);
	ItemBoxes[index]->MainCanvas->SetVisibility(ESlateVisibility::Hidden);
	ActiveItem = index;
	UUniformGridSlot* BoxSlot = Cast<UUniformGridSlot>(HighlightedItemBox->Slot);
	BoxSlot->SetColumn(index);
	HighlightedItemBox->SetVisibility(ESlateVisibility::Visible);
	if (ItemBoxes[index]->Item)
	{
		HighlightedItemBox->Image->SetBrushFromTexture(ItemBoxes[index]->Item->Thumbnail);
		HighlightedItemBox->Image->SetVisibility(ESlateVisibility::Visible);
		HighlightedItemBox->Count->SetVisibility(ESlateVisibility::Visible);
		HighlightedItemBox->Count->SetText(ItemBoxes[index]->Count->GetText());
	}
	else
	{
		HighlightedItemBox->Image->SetVisibility(ESlateVisibility::Hidden);
		HighlightedItemBox->Count->SetVisibility(ESlateVisibility::Hidden);
	}
}
