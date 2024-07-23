// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileObject.generated.h"

class UEquipmentComponent;

UCLASS(Abstract, BlueprintType, meta = (BlueprintSpawnableComponent))
class THIRDGAME_API AProjectileObject : public AActor
{
	GENERATED_BODY()

public:
	static TObjectPtr<AProjectileObject> Create(TObjectPtr<USceneComponent> AttachRoot, TObjectPtr<UEquipmentComponent> Equipment, const FVector& Location, const FRotator& Rotator, TSubclassOf<AProjectileObject> SubClass);
	
public:	
	// Sets default values for this actor's properties
	AProjectileObject();

	void FireWithArc(const FVector& TargetLocation, float Arc);

	virtual void InitProjectile(const FVector& Location, const FRotator& Rotator) PURE_VIRTUAL(AProjectileObject::InitProjectile, );

protected:
	void SetDisableTimer();
	void DeleteProjectile();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) PURE_VIRTUAL(AProjectileObject::OnBeginOverlap, );


protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float AutoDisableTime = 5.0f;

	TObjectPtr<UEquipmentComponent> FireEquipment = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UShapeComponent> ShapeComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> PMComponent = nullptr;

	FTimerHandle AutoDisableTimer;
	FScriptDelegate Delegate_OnHit;
};
