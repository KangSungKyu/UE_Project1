// Fill out your copyright notice in the Description page of Project Settings.


#include "ElitesKatanaComponent.h"
#include "UObject/UObjectGlobals.h"
#include "GameUnit.h"

UElitesKatanaComponent::UElitesKatanaComponent()
{
	EquipmentType = EQUIPMENT_TYPE::WEAPON;
	State = 0;
	ChargeState = 0;
	StateEnd = 9;
	ChargeCycleCount = 1;
	ChargeStateEnd = 5 * ChargeCycleCount;
	NormalCost = 20;
	ChargeCost = 35;
}

void UElitesKatanaComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	const FString MeshPath = TEXT("StaticMesh'/Game/HandsomeKatanaAnims/Weapon/Katana.Katana'");
	const FString MontagePath = TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_EliteKatana_AttackList.MN_EliteKatana_AttackList'");
	const FString ChargeMontagePath = TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_EliteKatana_ComboList.MN_EliteKatana_ComboList'");

	UStaticMesh* TempMesh = MyGameConfig::LoadObject<UStaticMesh>(*MeshPath);

	if (TempMesh != nullptr)
	{
		EquipmentMesh->SetStaticMesh(TempMesh);
		EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
		EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(5, 2.5f, 50));
	BoxComponent->SetRelativeLocationAndRotation(FVector(-5, 0, -50), FRotator(-7.5, 0, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	SetRelativeScale3D(FVector::OneVector * 1.5f);

	NormalMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath, nullptr, LOAD_None, nullptr);
	ChargeMontage = LoadObject<UAnimMontage>(nullptr, *ChargeMontagePath, nullptr, LOAD_None, nullptr);
}

void UElitesKatanaComponent::PlayStart()
{
	bPlay = true;
	State = FMath::RandRange(0, StateEnd - 1);

	Play();
}

void UElitesKatanaComponent::PlayChargeStart()
{
	bPlayCharge = true;
	ChargeState = FMath::RandRange(0, ChargeStateEnd - 1);

	PlayCharge();
}

void UElitesKatanaComponent::End(float BlendOut)
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

void UElitesKatanaComponent::ChargeEnd(bool bInitState)
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

void UElitesKatanaComponent::PlayCombo(const FString& NextSection)
{
}

void UElitesKatanaComponent::PlayChargeCombo()
{
}


void UElitesKatanaComponent::Play(const FString& NextSection)
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

void UElitesKatanaComponent::PlayCharge()
{
	if (0 <= ChargeState && ChargeState < ChargeStateEnd)
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

void UElitesKatanaComponent::CalcChargeState(bool bForceAttack)
{
}
