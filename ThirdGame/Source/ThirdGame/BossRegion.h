// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossRegion.generated.h"

class AEnemy;
class USphereComponent;
class AEmptyActor;

UCLASS(BlueprintType)
class THIRDGAME_API ABossRegion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossRegion();

	virtual void BeginPlay() override;

	float GetRegionRadius();

private:
	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPlayedEntrance = false;

	UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
	FVector EnterPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> Region = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> CenterOfRegionMeshes = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AEnemy>> LinkedEnemyList;

	FScriptDelegate Delegate_BeginOverlap;
	FScriptDelegate Delegate_EndOverlap;
};
