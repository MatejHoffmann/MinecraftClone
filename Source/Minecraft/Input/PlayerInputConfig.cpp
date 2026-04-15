// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInputConfig.h"

const UInputAction* UPlayerInputConfig::FindInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTaggedInputAction& TaggedInputAction : TaggedInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}
	return nullptr;
}
