// Fill out your copyright notice in the Description page of Project Settings.


#include "EmptyHandThrowerComponent.h"
#include "GameUnit.h"
#include "Enemy.h"
#include "SimpleRockProjectile.h"

UEmptyHandThrowerComponent::UEmptyHandThrowerComponent()
{
	EquipmentType = EQUIPMENT_TYPE::WEAPON;
	State = 0;
	ChargeState = 0;
	StateEnd = ATTACK_COMBO_UNIT * 1;
	ChargeCycleCount = 0;
	ChargeStateEnd = CHARGE_COMBO_UNIT * ChargeCycleCount;
	NormalCost = 50;
	ChargeCost = 65;
}

void UEmptyHandThrowerComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	const FString MontagePath = TEXT("AnimMontage'/Game/Characters/Animations/MN_Enemy_NormalHandThrower_Attack.MN_Enemy_NormalHandThrower_Attack'");

	EquipmentMesh->SetStaticMesh(nullptr);
	EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(10, 10, 10));
	BoxComponent->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	NormalMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath, nullptr, LOAD_None, nullptr);
}

void UEmptyHandThrowerComponent::PlayStart()
{
	bPlay = true;
	State = FMath::RandRange(0, StateEnd - 1);

	Play();
}

void UEmptyHandThrowerComponent::End(float BlendOut)
{
	bPlay = false;
	bContinue = false;

	bPlayCharge = false;
	bContinueCharge = false;

	State = 0;
	ChargeState = CHARGE_COMBO_STEP_START;

	TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(BlendOut, NormalMontage);
	}

	if (OwnerCharacter->GetTargetCharacter() != nullptr)
	{
		UWorld* World = GetWorld();

		if (World != nullptr)
		{
			FVector SpawnLocation;
			FRotator SpawnRotator;

			OwnerCharacter->GetMesh()->GetSocketWorldLocationAndRotation(*GetEquipSocketName(), SpawnLocation, SpawnRotator);

			SpawnLocation = SpawnLocation + OwnerCharacter->GetActorForwardVector() * 10;

			TObjectPtr<AProjectileObject> Projectile = AProjectileObject::Create(this, this, SpawnLocation, SpawnRotator, ASimpleRockProjectile::StaticClass());

			if (Projectile != nullptr)
			{
				// 발사체의 초기 탄도를 설정합니다.
				FVector TargetLocation = OwnerCharacter->GetTargetCharacter()->GetActorLocation();
				float AttackRange = 100.0f;
				TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OwnerCharacter);
				
				if (Enemy != nullptr)
				{
					AttackRange = Enemy->GetAttackRange();
				}

				float ArcRatio = 1.0f - FMath::Clamp(FVector::Dist(SpawnLocation, TargetLocation) / AttackRange, 0.05f, 0.45f);

				Projectile->FireWithArc(TargetLocation, ArcRatio);

				//UE_LOG(LogTemp, Log, TEXT("%.2f, %.2f, %.2f"), AttackRange, FVector::Dist(SpawnLocation, TargetLocation), ArcRatio);
			}
		}
	}
}

void UEmptyHandThrowerComponent::PlayCombo(const FString& NextSection)
{
}

void UEmptyHandThrowerComponent::PlayChargeCombo()
{
}

void UEmptyHandThrowerComponent::ChargeEnd(bool bInitState)
{
}

void UEmptyHandThrowerComponent::Play(const FString& NextSection)
{
	if (0 <= State && State < StateEnd)
	{
		Delegate_UseStamina.Broadcast(-NormalCost);

		TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		bool bAvailablePlay = true;

		if (AnimInstance != nullptr && IsAvailable())
		{
			FString AnimSection = FString::Printf(TEXT("Attack%d"), State + 1);

			if (!NextSection.IsEmpty())
			{
				AnimSection = NextSection;
			}

			if (NormalMontage != nullptr && NormalMontage->IsValidSectionName(*AnimSection))
			{
				AnimInstance->Montage_Play(NormalMontage);
				AnimInstance->Montage_JumpToSection(*AnimSection, NormalMontage);
			}

			OwnerCharacter->SetHitReaction(false);
		}
	}
}

void UEmptyHandThrowerComponent::PlayCharge()
{
}

void UEmptyHandThrowerComponent::CalcChargeState(bool bForceAttack)
{
}
