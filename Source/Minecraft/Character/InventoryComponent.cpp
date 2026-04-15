// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Items.Init({nullptr, 0}, 9);
}

void UInventoryComponent::HandleWheelScroll(const FInputActionValue& ActionValue)
{
	float Value = ActionValue.Get<float>();
	int Index = ActiveSlot + Value;
	ActiveSlot = (Index + 9) % 9;
	OnActiveSlotChange.ExecuteIfBound(ActiveSlot);
}

const UBlockItem* UInventoryComponent::GetActiveItem() const
{
	return Items[ActiveSlot].Key;
}

void UInventoryComponent::AddItem(UBlockItem* Item, int32 SlotIndex)
{
	Items[SlotIndex] = {Item, 0};
	OnActionBarItemChange.ExecuteIfBound(Item, SlotIndex);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	AActor* OwnerActor = GetOwner();
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
