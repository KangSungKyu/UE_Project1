// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "MyGameConfig.h"
#include "CharacterHeavyComboNotify.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UCharacterHeavyComboNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UCharacterHeavyComboNotify();

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	void Notify(USkeletalMeshComponent* MeshComp);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChargeAble = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQUIP_HAND_TYPE HandType = EQUIP_HAND_TYPE::RIGHT_HAND;

private:
	bool bChanged = false;
	
};
