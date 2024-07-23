// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HideCollisionNotify.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UHideCollisionNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UHideCollisionNotify();

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	ECollisionEnabled::Type CollisionType = ECollisionEnabled::Type::QueryAndPhysics;
	FName OriginUnitTag;
};
