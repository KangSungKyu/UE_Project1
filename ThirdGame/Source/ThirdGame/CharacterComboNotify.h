// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MyGameConfig.h"
#include "CharacterComboNotify.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UCharacterComboNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UCharacterComboNotify();
		
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NextSection = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQUIP_HAND_TYPE HandType = EQUIP_HAND_TYPE::RIGHT_HAND;

private:
	bool bChanged = false;
};
