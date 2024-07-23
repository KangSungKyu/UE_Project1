// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void CreatePoolList();

	UFUNCTION(BlueprintCallable)
	void DeletePoolList();

	void CreatePool(UClass* PoolClass, int Capacity = 100);
	TObjectPtr<AActor> Get(UClass* PoolClass);
	void Release(const UClass* PoolClass, TObjectPtr<AActor> Obj);

private:
	TMap<FName, TArray<TObjectPtr<AActor>>> ObjectPool;
};
