// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionStateNotify.h"
#include "GameUnit.h"
#include "EquipmentComponent.h"

UCollisionStateNotify::UCollisionStateNotify()
{
	bUseEquipmentR = false;
	bUseEquipmentL = false;
	bCheckSweep = true;
	bCheckHit = true;
}

void UCollisionStateNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AGameUnit* Unit = Cast<AGameUnit>(AnimInstance->GetOwningActor());

	if (Unit != nullptr)
	{
		bCheckHit = true;

		if (bUseEquipmentR)
		{
			OnCollision(Unit->GetEquipmentR());
		}

		if (bUseEquipmentL)
		{
			OnCollision(Unit->GetEquipmentL());
		}
	}
}

void UCollisionStateNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AGameUnit* Unit = Cast<AGameUnit>(AnimInstance->GetOwningActor());

	if (Unit != nullptr)
	{
		if (bUseEquipmentR)
		{
			OnCollision(Unit->GetEquipmentR());
		}

		if (bUseEquipmentL)
		{
			OnCollision(Unit->GetEquipmentL());
		}
	}
}

void UCollisionStateNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	AGameUnit* Unit = Cast<AGameUnit>(AnimInstance->GetOwningActor());

	if (Unit != nullptr)
	{
		if (bUseEquipmentR)
		{
			OnCollision(Unit->GetEquipmentR());
		}

		if (bUseEquipmentL)
		{
			OnCollision(Unit->GetEquipmentL());
		}

		OffCollision(Unit->GetEquipmentR());
		OffCollision(Unit->GetEquipmentL());
	}
}

void UCollisionStateNotify::OnCollision(TObjectPtr<UEquipmentComponent> Equipment)
{
	if (Equipment != nullptr)
	{
		Equipment->SetBoxCollision(true);

		if (bCheckSweep && bCheckHit)
		{
			FHitResult HitResult;
			bool bResult = Equipment->CheckSweepCollision(HitResult);

			if (bResult && bCheckFirstHit)
			{
				bCheckHit = false;
			}
		}
	}
}

void UCollisionStateNotify::OffCollision(TObjectPtr<UEquipmentComponent> Equipment)
{
	if (Equipment != nullptr)
	{
		Equipment->SetBoxCollision(false);
	}
}