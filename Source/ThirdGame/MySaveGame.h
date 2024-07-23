// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMySaveGame();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	FString LevelStartTag;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

};
