// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameConfig.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CollisionStateNotify.generated.h"

class AGameUnit;
class UEquipmentComponent;
/**
 * 
 */
UCLASS()
class THIRDGAME_API UCollisionStateNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UCollisionStateNotify();

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	void OnCollision(TObjectPtr<UEquipmentComponent> Equipment);
	void OffCollision(TObjectPtr<UEquipmentComponent> Equipment);

private:
	UPROPERTY(EditAnywhere)
	bool bUseEquipmentR = false;

	UPROPERTY(EditAnywhere)
	bool bUseEquipmentL = false;

	UPROPERTY(EditAnywhere)
	bool bCheckSweep = true;

	UPROPERTY(EditAnywhere)
	bool bCheckFirstHit = true;

	bool bCheckHit = true;
};
