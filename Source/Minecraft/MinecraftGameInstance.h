// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "World/MinecraftWorldSettings.h"
#include "MinecraftGameInstance.generated.h"

UCLASS()
class MINECRAFT_API UMinecraftGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/*
	 * Returns all available saved worlds
	 */
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAvailableSaveGameFiles();

	/*
	 * Returns available world name, that starts as New World * (* = available number from lowest)
	 */
	FString GetAvailableSaveGameFileName();

	/*
	 * Checks whether the given file name is available for use
	 */
	bool IsSaveGameFileNameAvailable(const FString& FileName);

	/*
	 * Deletes the given game file. It exists return true otherwise false
	 */
	bool DeleteSaveGameFile(const FString& FileName);

	/*
	 * Saves game data information
	 */
	void SetGameData(const FString& Name, int32 Seed, bool New);

	/*
	 * Sets render distance
	 */
	void SetRenderDistance(int Distance);

protected:
	virtual void Init() override;

private:
	FString SaveGameDir;
	FString SaveGameDirPath;
	FString SaveGameFileExt;
	FString SaveGameFileExtDot;

public:
	UPROPERTY(BlueprintReadOnly)
	bool bNewWorld = true;
	UPROPERTY(BlueprintReadOnly)
	FString WorldName;
	UPROPERTY(BlueprintReadOnly)
	int32 WorldSeed;
	UPROPERTY(BlueprintReadOnly)
	int32 RenderDistance = DEFAULT_RENDER_DISTANCE;
};
