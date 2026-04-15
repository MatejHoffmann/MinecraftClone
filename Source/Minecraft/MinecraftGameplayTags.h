// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "GameplayTagContainer.h"


struct FMinecraftGameplayTags
{
public:
	static const FMinecraftGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Fly;
	FGameplayTag InputTag_LeftHand;
	FGameplayTag InputTag_RightHand;
	FGameplayTag InputTag_FlyMode;
	FGameplayTag InputTag_Swim;
	FGameplayTag InputTag_Crouch;

	FGameplayTag InputTag_Menu;

	FGameplayTag InputTag_ChangeItem;
	FGameplayTag InputTag_ScrollItem;

protected:
	void AddAllTags(UGameplayTagsManager& TagsManager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:
	static FMinecraftGameplayTags GameplayTags;
};
