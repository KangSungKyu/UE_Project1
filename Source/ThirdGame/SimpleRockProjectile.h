// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileObject.h"
#include "SimpleRockProjectile.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API ASimpleRockProjectile : public AProjectileObject
{
	GENERATED_BODY()
	
public:
	ASimpleRockProjectile();

	virtual void InitProjectile(const FVector& Location, const FRotator& Rotator) override;

protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;
};
