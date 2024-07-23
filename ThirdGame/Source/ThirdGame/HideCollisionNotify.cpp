// Fill out your copyright notice in the Description page of Project Settings.


#include "HideCollisionNotify.h"
#include "GameUnit.h"
#include "Components/CapsuleComponent.h"

UHideCollisionNotify::UHideCollisionNotify()
{
	CollisionType = ECollisionEnabled::Type::QueryAndPhysics;
	OriginUnitTag = TEXT("");
}

void UHideCollisionNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	AGameUnit* Unit = Cast<AGameUnit>(MeshComp->GetOwner());

	if (Unit != nullptr)
	{
		UCapsuleComponent* UnitCapsule = Unit->GetCapsuleComponent();

		CollisionType = UnitCapsule->GetCollisionEnabled();
		OriginUnitTag = Unit->GetUnitTag();

		UnitCapsule->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		Unit->Tags.Remove(OriginUnitTag);
	}
}

void UHideCollisionNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	AGameUnit* Unit = Cast<AGameUnit>(MeshComp->GetOwner());

	if (Unit != nullptr)
	{
		UCapsuleComponent* UnitCapsule = Unit->GetCapsuleComponent();

		ECollisionEnabled::Type CurrType = UnitCapsule->GetCollisionEnabled();

		if (CurrType != ECollisionEnabled::Type::QueryOnly)
		{
			UnitCapsule->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		}

		if (Unit->ActorHasTag(OriginUnitTag))
		{
			Unit->Tags.Remove(OriginUnitTag);
		}
	}
}

void UHideCollisionNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	AGameUnit* Unit = Cast<AGameUnit>(MeshComp->GetOwner());

	if (Unit != nullptr)
	{
		UCapsuleComponent* UnitCapsule = Unit->GetCapsuleComponent();

		UnitCapsule->SetCollisionEnabled(CollisionType);
		Unit->Tags.Add(OriginUnitTag);
	}
}
