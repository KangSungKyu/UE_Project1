// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldComponent.h"
#include "UObject/UObjectGlobals.h"
#include "GameUnit.h"

UShieldComponent::UShieldComponent()
{
	EquipmentType = EQUIPMENT_TYPE::SHIELD;
	State = 0;
	ChargeState = 0;
	StateEnd = 2;
	ChargeCycleCount = 0;
	ChargeStateEnd = 0;
	NormalCost = 10;
	ChargeCost = 35;
}

void UShieldComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	const FString MeshPath = TEXT("StaticMesh'/Game/ARPG_Warrior/Demo/Weapon/Mesh/Shield.Shield'");

	TObjectPtr<UStaticMesh> TempMesh = MyGameConfig::LoadObject<UStaticMesh>(*MeshPath);

	if (TempMesh != nullptr)
	{
		EquipmentMesh->SetStaticMesh(TempMesh);
		EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(-90, -90, 0));
		EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(60, 60, 10));
	BoxComponent->SetRelativeLocationAndRotation(FVector(0, 00, 0), FRotator(-90, -90, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	//NormalMontage = LoadObject<UAnimMontage>(nullptr, *MontagePath, nullptr, LOAD_None, nullptr);
	//ChargeMontage = LoadObject<UAnimMontage>(nullptr, *ChargeMontagePath, nullptr, LOAD_None, nullptr);

	BoxComponent->SetCollisionProfileName(TEXT("Guardable"));

	End(0.0f);
}

void UShieldComponent::Play(const FString& NextSection)
{
	State = 0;

	OwnerCharacter->CheckShield();
	OwnerCharacter->CalcMoveSpeed();
	OwnerCharacter->SetHitReaction(false);
}

void UShieldComponent::PlayCharge()
{
}

void UShieldComponent::CalcChargeState(bool bForceAttack)
{
}

void UShieldComponent::PlayStart()
{
	Play();
}

void UShieldComponent::PlayTrigger()
{
	if (OwnerCharacter->IsOnShield())
	{
		Play();
	}
}

void UShieldComponent::PlayCompleted()
{
	End(1.0f);
}

void UShieldComponent::PlayChargeStart()
{
	if (State == StateEnd)
	{
		Delegate_UseStamina.Broadcast(-ChargeCost);

		State = 1;

		OwnerCharacter->CheckShield();
		OwnerCharacter->CalcMoveSpeed();
		OwnerCharacter->SetHitReaction(false);
	}
}

void UShieldComponent::ChargeEnd(bool bInitState)
{
	End(1.0f);
}

void UShieldComponent::End(float BlendOut)
{
	State = StateEnd;

	OwnerCharacter->CheckShield();
	OwnerCharacter->CalcMoveSpeed();
}

void UShieldComponent::PlayCombo(const FString& NextSection)
{
}

void UShieldComponent::PlayChargeCombo()
{
}
