// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "PlayerInputConfig.h"
#include "GameplayTagContainer.h"
#include "MinecraftInputComponent.generated.h"


UCLASS()
class MINECRAFT_API UMinecraftInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	/*
	 * Bind action by given tag
	 */
	template <class UserClass, typename FuncType>
	void BindActionByTag(const UPlayerInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object,
	                     FuncType Func);
};

template <class UserClass, typename FuncType>
void UMinecraftInputComponent::BindActionByTag(const UPlayerInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent,
                                               UserClass* Object, FuncType Func)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
