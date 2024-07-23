// Fill out your copyright notice in the Description page of Project Settings.


#include "EmptyOneHandSwordComponent.h"
#include "GameUnit.h"

UEmptyOneHandSwordComponent::UEmptyOneHandSwordComponent()
{
	EquipmentType = EQUIPMENT_TYPE::WEAPON;
	State = 0;
	ChargeState = 0;
	StateEnd = ATTACK_COMBO_UNIT * 2;
	ChargeCycleCount = 1;
	ChargeStateEnd = CHARGE_COMBO_UNIT * ChargeCycleCount;
	NormalCost = 10;
	ChargeCost = 15;
}

void UEmptyOneHandSwordComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	const FString MontagePath = TEXT("AnimMontage'/Game/Characters/Animations/MN_NormalWarrior_Attack.MN_NormalWarrior_Attack'");
	const FString ChargeMontagePath = TEXT("AnimMontage'/Game/Characters/Animations/MN_NormalWarrior_HeavyAttack.MN_NormalWarrior_HeavyAttack'");

	EquipmentMesh->SetStaticMesh(nullptr);
	EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(-90, -90, 0));
	EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	//BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(55, 1, 5));
	BoxComponent->SetRelativeLocationAndRotation(FVector(0, 0, 80), FRotator(-90, -90, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	NormalMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath, nullptr, LOAD_None, nullptr);
	ChargeMontage = LoadObject<UAnimMontage>(nullptr, *ChargeMontagePath, nullptr, LOAD_None, nullptr);
}

void UEmptyOneHandSwordComponent::PlayStart()
{
	bPlay = true;
	State = FMath::RandRange(0, StateEnd - 1);

	Play();
}

void UEmptyOneHandSwordComponent::PlayChargeStart()
{
	bPlayCharge = true;
	ChargeState = CHARGE_COMBO_STEP_ATTACK;

	PlayCharge();
}

void UEmptyOneHandSwordComponent::End(float BlendOut)
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
}

void UEmptyOneHandSwordComponent::ChargeEnd(bool bInitState)
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

void UEmptyOneHandSwordComponent::PlayCombo(const FString& NextSection)
{
}

void UEmptyOneHandSwordComponent::PlayChargeCombo()
{
}

void UEmptyOneHandSwordComponent::Play(const FString& NextSection)
{
	if (0 <= State && State < StateEnd)
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

void UEmptyOneHandSwordComponent::PlayCharge()
{
	if (0 <= ChargeState && ChargeState < ChargeStateEnd)
	{
		Delegate_UseStamina.Broadcast(-ChargeCost);

		TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

		if (AnimInstance != nullptr && IsAvailableCharge())
		{
			FString AnimSection = "";
			uint8 AnimIdx = (ChargeState / CHARGE_COMBO_UNIT) + 1;
			uint8 StateIdx = ChargeState % CHARGE_COMBO_UNIT;

			if (StateIdx == CHARGE_COMBO_STEP_START)
			{
				AnimSection = FString::Printf(TEXT("Start%d"), AnimIdx);
			}
			else if (StateIdx == CHARGE_COMBO_STEP_CHARGE)
			{
				AnimSection = FString::Printf(TEXT("Charge%d"), AnimIdx);
			}
			else if (StateIdx == CHARGE_COMBO_STEP_ATTACK)
			{
				AnimSection = FString::Printf(TEXT("Attack%d"), AnimIdx);
			}

			if (ChargeMontage != nullptr && ChargeMontage->IsValidSectionName(*AnimSection))
			{
				AnimInstance->Montage_Play(ChargeMontage);
				AnimInstance->Montage_JumpToSection(*AnimSection, ChargeMontage);
			}

			OwnerCharacter->SetHitReaction(false);
		}
	}
}

void UEmptyOneHandSwordComponent::CalcChargeState(bool bForceAttack)
{
}
