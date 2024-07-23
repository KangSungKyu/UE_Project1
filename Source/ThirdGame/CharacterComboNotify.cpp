// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComboNotify.h"
#include "MyCharacter.h"
#include "EquipmentComponent.h"

UCharacterComboNotify::UCharacterComboNotify()
{
	bChanged = false;
}

void UCharacterComboNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AMyCharacter* Character = Cast<AMyCharacter>(AnimInstance->GetOwningActor());

	if (Character && Character->IsEquipmentContinue(HandType) && !bChanged)
	{
		bChanged = true;
	}
}

void UCharacterComboNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AMyCharacter* Character = Cast<AMyCharacter>(AnimInstance->GetOwningActor());

	if (Character && Character->IsEquipmentContinue(HandType) && !bChanged)
	{
		bChanged = true;
	}
}

void UCharacterComboNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AMyCharacter* Character = Cast<AMyCharacter>(AnimInstance->GetOwningActor());

	if (Character)
	{
		if (!Character->IsHitReaction() && (Character->IsEquipmentContinue(HandType) || bChanged))
		{
			Character->ComboAttack(HandType, NextSection);
		}
		else
		{
			Character->AttackEnd(HandType, 1.0f);
		}
	}
}
