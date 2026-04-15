// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Minecraft/Items/Item.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Minecraft/Items/BlockItem.h"
#include "InventoryComponent.generated.h"


DECLARE_DELEGATE_TwoParams(FActionBarItemDelegate, UItem* Item, int32 Index);
DECLARE_DELEGATE_OneParam(FActiveSlotDelegate, int32 SlotIndex);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINECRAFT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	 * On Mouse wheel scroll change active item
	 */
	void HandleWheelScroll(const FInputActionValue& ActionValue);

	/*
	 * Return currently active item
	 */
	const UBlockItem* GetActiveItem() const;

	/*
	 * Add item to the action bar at the given slot
	 */
	UFUNCTION(BlueprintCallable)
	void AddItem(UBlockItem* Item, int32 SlotIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;

public:
	// Called every frame
	FActionBarItemDelegate OnActionBarItemChange;
	FActiveSlotDelegate OnActiveSlotChange;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ScrollAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> ActionBarInputMapping;

private:
	TArray<TPair<UBlockItem*, int32>> Items;
	int32 ActiveSlot;
};
