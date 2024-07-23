// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyRegion.generated.h"

class AEnemy;

UCLASS(BlueprintType)
class THIRDGAME_API AEnemyRegion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyRegion();

	float GetRegionRadius();
	void LinkEnemy(const TObjectPtr<AEnemy>& Enemy);

private:
	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> Region = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AEnemy>> LinkedEnemyList;

	FScriptDelegate Delegate_BeginOverlap;
	FScriptDelegate Delegate_EndOverlap;
};
