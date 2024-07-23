// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"
#include "ProjectileObject.h"
#include "SimpleRockProjectile.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UObjectPoolSubsystem::Deinitialize()
{
	Super::Deinitialize();

	DeletePoolList();
}

void UObjectPoolSubsystem::CreatePoolList()
{
	CreatePool(ASimpleRockProjectile::StaticClass(), 10);
}

void UObjectPoolSubsystem::DeletePoolList()
{
	TArray<FName> KeyList;
	ObjectPool.GetKeys(KeyList);
	//for (auto& PoolItr : ObjectPool)
	for(int i = 0; i < ObjectPool.Num(); ++i)
	{
		auto& PoolItr = ObjectPool[KeyList[i]];

		for (int j = 0; j < PoolItr.Num(); ++j)
		{
			auto& ListItr = PoolItr[j];

			if (ListItr != nullptr && ListItr->IsValidLowLevel())
			{
				ListItr->Destroy();
			}
		}

		ObjectPool.Remove(KeyList[i]);
	}

	ObjectPool.Reset();
}


void UObjectPoolSubsystem::CreatePool(UClass* PoolClass, int Capacity)
{
	FName PoolName = PoolClass->GetFName();

	if (!ObjectPool.Contains(PoolName))
	{
		TObjectPtr<UWorld> World = GetWorld();
		TArray<TObjectPtr<AActor>> PoolList;

		for (int i = 0; i < Capacity; ++i)
		{
			FString CreatedName = FString::Printf(TEXT("%s_%d"), *PoolName.ToString(), i);
			TObjectPtr<AActor> CreatedObj = nullptr;
			FActorSpawnParameters SpawnParams;

			CreatedObj = World->SpawnActor<AActor>(PoolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

			CreatedObj->SetActorHiddenInGame(true);

			PoolList.Add(CreatedObj);
		}

		ObjectPool.Add(PoolName, PoolList);

		UE_LOG(LogTemp, Log, TEXT("Created ObjectPool %s, Capacity : %d"), *PoolName.ToString(), Capacity);
	}
}

TObjectPtr<AActor> UObjectPoolSubsystem::Get(UClass* PoolClass)
{
	TObjectPtr<AActor> InPool = nullptr;
	FName PoolName = PoolClass->GetFName();
	bool bCreatedPool = ObjectPool.Contains(PoolName);

	if (!bCreatedPool || ObjectPool.Find(PoolName)->IsEmpty())
	{
		CreatePool(PoolClass, 10);
	}

	if (bCreatedPool)
	{
		TArray<TObjectPtr<AActor>>* PoolList = ObjectPool.Find(PoolName);

		if (PoolList != nullptr)
		{
			int Idx = PoolList->IndexOfByPredicate([&](const AActor* Itr)
			{
				return Itr->IsHidden();
			});

			if (PoolList->IsValidIndex(Idx))
			{
				InPool = (*PoolList)[Idx];

				InPool->SetActorHiddenInGame(false);
				PoolList->Remove(InPool);
			}
		}
	}

	return InPool;
}

void UObjectPoolSubsystem::Release(const UClass* PoolClass, TObjectPtr<AActor> Obj)
{
	FName PoolName = PoolClass->GetFName();

	if (!ObjectPool.Contains(PoolName))
	{
		TArray<TObjectPtr<AActor>> PoolList;

		ObjectPool.Add(PoolName, PoolList);
	}

	TArray<TObjectPtr<AActor>>* PoolList = ObjectPool.Find(PoolName);

	if (PoolList != nullptr)
	{
		Obj->SetActorHiddenInGame(true);
		PoolList->Add(Obj);
	}
}
