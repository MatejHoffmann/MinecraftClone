// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftAssetManager.h"

#include "MinecraftGameplayTags.h"

UMinecraftAssetManager& UMinecraftAssetManager::Get()
{
	check(GEngine);
	UMinecraftAssetManager* MinecraftAssetManager = Cast<UMinecraftAssetManager>(GEngine->AssetManager);
	return *MinecraftAssetManager;
}

void UMinecraftAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FMinecraftGameplayTags::InitializeNativeTags();
}
