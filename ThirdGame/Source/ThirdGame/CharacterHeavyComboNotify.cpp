// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHeavyComboNotify.h"
#include "MyCharacter.h"


UCharacterHeavyComboNotify::UCharacterHeavyComboNotify()
{
	bChanged = false;
}

void UCharacterHeavyComboNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AMyCharacter* Character = Cast<AMyCharacter>(AnimInstance->GetOwningActor());

	if (Character && Character->IsEquipmentContinueCharge(HandType) && !bChanged)
	{
		bChanged = true;
	}
}

void UCharacterHeavyComboNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AMyCharacter* Character = Cast<AMyCharacter>(AnimInstance->GetOwningActor());

	if (Character && Character->IsEquipmentContinueCharge(HandType) && !bChanged)
	{
		bChanged = true;
	}
}

void UCharacterHeavyComboNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	Notify(MeshComp);
}

void UCharacterHeavyComboNotify::Notify(USkeletalMeshComponent* MeshComp)
{
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AMyCharacter* Character = Cast<AMyCharacter>(AnimInstance->GetOwningActor());

	if (Character)
	{
		bool bNext = Character->IsEquipmentContinueCharge(HandType) || bChanged;

		if (bNext)
		{
			Character->ComboChargeAttack(HandType);
		}
		else if (!bNext && !bChargeAble)
		{
			Character->ChargeAttackEnd(HandType);
		}
	}
}
