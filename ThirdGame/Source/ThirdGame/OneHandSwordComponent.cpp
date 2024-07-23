// Fill out your copyright notice in the Description page of Project Settings.


#include "OneHandSwordComponent.h"
#include "UObject/UObjectGlobals.h"
#include "GameUnit.h"

UOneHandSwordComponent::UOneHandSwordComponent()
{
	EquipmentType = EQUIPMENT_TYPE::WEAPON;
	State = 0;
	ChargeState = 0;
	StateEnd = ATTACK_COMBO_UNIT * 4;
	ChargeCycleCount = 2;
	ChargeStateEnd = CHARGE_COMBO_UNIT * ChargeCycleCount;
	NormalCost = 20;
	ChargeCost = 35;
}

void UOneHandSwordComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	const FString MeshPath = TEXT("StaticMesh'/Game/ARPG_Warrior/Demo/Weapon/Mesh/Sword.Sword'");
	const FString MontagePath = TEXT("AnimMontage'/Game/Characters/Animations/MN_Character_Attack.MN_Character_Attack'");
	const FString MontagePath2 = TEXT("AnimMontage'/Game/Characters/Animations/MN_Character_Attack2.MN_Character_Attack2'");
	const FString ChargeMontagePath = TEXT("AnimMontage'/Game/Characters/Animations/MN_Character_HeavyAttack.MN_Character_HeavyAttack'");

	UStaticMesh* TempMesh = MyGameConfig::LoadObject<UStaticMesh>(*MeshPath);

	if (TempMesh != nullptr)
	{
		EquipmentMesh->SetStaticMesh(TempMesh);
		EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(-90, -90, 0));
		EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(5, 55, 1));
	BoxComponent->SetRelativeLocationAndRotation(FVector(-48, 0, 0), FRotator(-90, -90, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	FString SelectedMontagePath = MontagePath;

	if (EquipHandType == EQUIP_HAND_TYPE::LEFT_HAND)
	{
		SelectedMontagePath = MontagePath2;
	}

	NormalMontage = LoadObject<UAnimMontage>(nullptr, *SelectedMontagePath, nullptr, LOAD_None, nullptr);
	ChargeMontage = LoadObject<UAnimMontage>(nullptr, *ChargeMontagePath, nullptr, LOAD_None, nullptr);
}

void UOneHandSwordComponent::PlayStart()
{
	if (bPlay && State < StateEnd)
	{
		bContinue = true;
	}
	else
	{
		bPlay = true;

		Play();
	}
}

void UOneHandSwordComponent::PlayChargeStart()
{
	if (IsAvailableCharge())
	{
		if (bPlayCharge && ChargeState < ChargeStateEnd)
		{
			bContinueCharge = true;
		}
		else
		{
			bPlayCharge = true;

			CalcChargeState();
			PlayCharge();
		}
	}
	else
	{
		ChargeEnd(false);
	}
}

void UOneHandSwordComponent::PlayChargeTrigger()
{
	if (bPlayCharge && (ChargeState % CHARGE_COMBO_UNIT == CHARGE_COMBO_STEP_START))
	{
		CalcChargeState();
		PlayCharge();
	}
}

void UOneHandSwordComponent::PlayChargeCanceled()
{
	if (bPlayCharge)
	{
		CalcChargeState(true);
		PlayCharge();
	}
}

void UOneHandSwordComponent::PlayChargeCompleted()
{
	if (bPlayCharge && (ChargeState % CHARGE_COMBO_UNIT == CHARGE_COMBO_STEP_CHARGE))
	{
		CalcChargeState();
		PlayCharge();
	}
}

void UOneHandSwordComponent::PlayCombo(const FString& NextSection)
{
	if (!bContinue)
	{
		End(1.0f);
	}
	else
	{
		Play(NextSection);
	}

	bContinue = false;
}

void UOneHandSwordComponent::PlayChargeCombo()
{
	if (!bContinueCharge)
	{
		ChargeEnd();
	}
	else
	{
		CalcChargeState();
		PlayCharge();
	}

	bContinueCharge = false;
}

void UOneHandSwordComponent::End(float BlendOut)
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

void UOneHandSwordComponent::ChargeEnd(bool bInitState)
{
	bool bCharging = ChargeState % CHARGE_COMBO_UNIT == CHARGE_COMBO_STEP_CHARGE;
	TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		if (bCharging)
		{
			CalcChargeState(true);
			PlayCharge();
		}
		else
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
}

void UOneHandSwordComponent::Play(const FString& NextSection)
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

			State = (State + 1) % StateEnd;

			OwnerCharacter->SetHitReaction(false);
		}
	}
}

void UOneHandSwordComponent::PlayCharge()
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

void UOneHandSwordComponent::CalcChargeState(bool bForceAttack)
{
	uint8 Prev = ChargeState;

	ChargeState = ChargeState + 1;

	if (bForceAttack)
	{
		uint8 ChargeIdx = ChargeState / CHARGE_COMBO_UNIT;
		uint8 ChargeComboStep = ChargeState % CHARGE_COMBO_UNIT;

		if (ChargeComboStep == CHARGE_COMBO_STEP_START || ChargeComboStep == CHARGE_COMBO_STEP_CHARGE)
		{
			ChargeState = ChargeIdx * CHARGE_COMBO_UNIT + CHARGE_COMBO_STEP_ATTACK;
		}
	}

	ChargeState = ChargeState % ChargeStateEnd;
}
