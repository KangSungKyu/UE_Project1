// Fill out your copyright notice in the Description page of Project Settings.


#include "BossyGreatSwordComponent.h"
#include "UObject/UObjectGlobals.h"
#include "GameUnit.h"

UBossyGreatSwordComponent::UBossyGreatSwordComponent()
{
	EquipmentType = EQUIPMENT_TYPE::WEAPON;
	State = 0;
	ChargeState = 0;
	StateEnd = 2;
	ChargeCycleCount = 1;
	ChargeStateEnd = 4 * ChargeCycleCount;
	NormalCost = 20;
	ChargeCost = 35;
}

void UBossyGreatSwordComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	const FString MontagePath = TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_BossGreatSword_AttackList.MN_BossGreatSword_AttackList'");
	const FString ChargeMontagePath = TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_BossGreatSword_ComboList.MN_BossGreatSword_ComboList'");
	
	EquipmentMesh->SetStaticMesh(nullptr);
	EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	//BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(2.5, 5.5f, 30));
	BoxComponent->SetRelativeLocationAndRotation(FVector(0, 0, 30), FRotator(0, 0, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	SetRelativeScale3D(FVector::OneVector * 1.5f);

	NormalMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath, nullptr, LOAD_None, nullptr);
	ChargeMontage = LoadObject<UAnimMontage>(nullptr, *ChargeMontagePath, nullptr, LOAD_None, nullptr);
}

void UBossyGreatSwordComponent::PlayStart()
{
	bPlay = true;
	State = FMath::RandRange(0, StateEnd - 1);

	Play();
}

void UBossyGreatSwordComponent::PlayChargeStart()
{
	bPlayCharge = true;
	ChargeState = FMath::RandRange(0, ChargeStateEnd - 1);

	PlayCharge();
}

void UBossyGreatSwordComponent::End(float BlendOut)
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
		AnimInstance->Montage_Stop(BlendOut, ChargeMontage);
	}
}

void UBossyGreatSwordComponent::ChargeEnd(bool bInitState)
{
	TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		bPlayCharge = false;
		bContinueCharge = false;

		if (bInitState)
		{
			ChargeState = CHARGE_COMBO_STEP_START;

			AnimInstance->Montage_Stop(0.5f, ChargeMontage);
		}
	}
}

void UBossyGreatSwordComponent::PlayCombo(const FString& NextSection)
{
}

void UBossyGreatSwordComponent::PlayChargeCombo()
{
}


void UBossyGreatSwordComponent::Play(const FString& NextSection)
{
	if (OwnerCharacter != nullptr && 0 <= State && State < StateEnd)
	{
		Delegate_UseStamina.Broadcast(-NormalCost);

		TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		bool bAvailablePlay = true;

		if (AnimInstance != nullptr && IsAvailable())
		{
			FString MoveState = TEXT("");

			if (OwnerCharacter->GetMovementState() == MOVEMENT_STATE::RUN)
			{
				MoveState = TEXT("Run");
				State = 0;
			}

			FString AnimSection = FString::Printf(TEXT("%sAttack%d"), *MoveState, State + 1);

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

void UBossyGreatSwordComponent::PlayCharge()
{
	if (OwnerCharacter != nullptr && 0 <= ChargeState && ChargeState < ChargeStateEnd)
	{
		Delegate_UseStamina.Broadcast(-ChargeCost);

		TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

		if (AnimInstance != nullptr && IsAvailableCharge())
		{
			FString AnimSection = FString::Printf(TEXT("Attack%d"), ChargeState + 1);

			if (ChargeMontage != nullptr && ChargeMontage->IsValidSectionName(*AnimSection))
			{
				AnimInstance->Montage_Play(ChargeMontage);
				AnimInstance->Montage_JumpToSection(*AnimSection, ChargeMontage);
			}

			OwnerCharacter->SetHitReaction(false);
		}
	}
}

void UBossyGreatSwordComponent::CalcChargeState(bool bForceAttack)
{
}


