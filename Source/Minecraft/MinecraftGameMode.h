// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "World/MinecraftWorld.h"
#include "GameFramework/GameModeBase.h"
#include "MinecraftGameMode.generated.h"

UCLASS()
class MINECRAFT_API AMinecraftGameMode : public AGameModeBase
{
	GENERATED_BODY()
	virtual void StartPlay() override;

public:
	UPROPERTY(BlueprintReadWrite)
	AMinecraftWorld* MinecraftWorld;
};
