// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftGameplayTags.h"

FMinecraftGameplayTags FMinecraftGameplayTags::GameplayTags;

void FMinecraftGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(GameplayTagsManager);
	GameplayTagsManager.DoneAddingNativeTags();
}

void FMinecraftGameplayTags::AddAllTags(UGameplayTagsManager& TagsManager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input");
	AddTag(InputTag_Look, "InputTag.Look", "Look input");
	AddTag(InputTag_Jump, "InputTag.Jump", "Jump input");
	AddTag(InputTag_Fly, "InputTag.Fly", "Fly input");
	AddTag(InputTag_LeftHand, "InputTag.LeftHand", "Left hand input");
	AddTag(InputTag_RightHand, "InputTag.RightHand", "Right hand input");
	AddTag(InputTag_FlyMode, "InputTag.FlyMode", "FlyMode input");
	AddTag(InputTag_Swim, "InputTag.Swim", "Swimming input");
	AddTag(InputTag_ChangeItem, "InputTag.ChangeItem", "Change item input");
	AddTag(InputTag_ScrollItem, "InputTag.ScrollItem", "Change item input");
	AddTag(InputTag_Crouch, "InputTag.Crouch", "Crouch input");
	AddTag(InputTag_Menu, "InputTag.Menu", "Open menu input");
}

void FMinecraftGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
