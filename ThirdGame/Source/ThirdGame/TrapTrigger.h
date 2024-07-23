// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapTrigger.generated.h"

class UBoxComponent;
class AGameUnit;

UCLASS()
class THIRDGAME_API ATrapTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> TriggerArea = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<AGameUnit>, FString> LinkedTriggerList;

	FScriptDelegate Delegate_BeginOverlap;
	FScriptDelegate Delegate_EndOverlap;
	FScriptDelegate_TrapTrigger Delegate_Trigger;
};
