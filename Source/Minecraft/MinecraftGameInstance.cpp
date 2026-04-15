// Fill out your copyright notice in the Description page of Project Settings.


#include "MinecraftGameInstance.h"
#include "Kismet/GameplayStatics.h"


void UMinecraftGameInstance::Init()
{
	Super::Init();
	SaveGameDir = TEXT("SaveGames");
	SaveGameDirPath = FPaths::ProjectSavedDir() + SaveGameDir;
	SaveGameFileExt = TEXT("sav");
	SaveGameFileExtDot = TEXT(".") + SaveGameFileExt;
}


TArray<FString> UMinecraftGameInstance::GetAvailableSaveGameFiles()
{
	const FString Empty = TEXT("");
	TArray<FString> FoundFiles;
	IFileManager::Get().FindFiles(FoundFiles, *SaveGameDirPath, *SaveGameFileExt);
	for (int i = 0; i < FoundFiles.Num(); i++)
	{
		FoundFiles[i] = FoundFiles[i].Replace(*SaveGameFileExtDot, *Empty);
	}
	return FoundFiles;
}

FString UMinecraftGameInstance::GetAvailableSaveGameFileName()
{
	TArray<FString> AvailableFiles = GetAvailableSaveGameFiles();
	int Num = 0;
	for (int i = 0; i < AvailableFiles.Num(); i++)
	{
		if (AvailableFiles[i].Contains("New World"))
		{
			Num++;
		}
	}
	FString FileName = "New World " + FString::FromInt(Num);
	while (!IsSaveGameFileNameAvailable(FileName))
	{
		Num++;
		FileName = "New World " + FString::FromInt(Num);
	}
	return FileName;
}

bool UMinecraftGameInstance::IsSaveGameFileNameAvailable(const FString& FileName)
{
	TArray<FString> AvailableFiles = GetAvailableSaveGameFiles();
	for (int i = 0; i < AvailableFiles.Num(); i++)
	{
		if (AvailableFiles[i] == FileName)
		{
			return false;
		}
	}
	return true;
}


bool UMinecraftGameInstance::DeleteSaveGameFile(const FString& FileName)
{
	return UGameplayStatics::DeleteGameInSlot(FileName, 0);
}

void UMinecraftGameInstance::SetGameData(const FString& Name, int32 Seed, bool New)
{
	bNewWorld = New;
	WorldName = Name;
	WorldSeed = Seed;
}

void UMinecraftGameInstance::SetRenderDistance(int Distance)
{
	RenderDistance = Distance;
}
