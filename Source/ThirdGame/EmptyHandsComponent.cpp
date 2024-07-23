// Fill out your copyright notice in the Description page of Project Settings.


#include "EmptyHandsComponent.h"
#include "UObject/UObjectGlobals.h"
#include "GameUnit.h"

void UEmptyHandsComponent::PlayCombo(const FString& NextSection)
{
}

void UEmptyHandsComponent::PlayChargeCombo()
{
}

void UEmptyHandsComponent::End(float BlendOut)
{
}

void UEmptyHandsComponent::ChargeEnd(bool bInitState)
{
}

UEmptyHandsComponent::UEmptyHandsComponent()
{
	EquipmentType = EQUIPMENT_TYPE::WEAPON;
	State = 0;
	ChargeState = 0;
	StateEnd = 1;
	ChargeCycleCount = 1;
	ChargeStateEnd = 0 * ChargeCycleCount;
	NormalCost = 20;
	ChargeCost = 35;
}

void UEmptyHandsComponent::InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh)
{
	EquipmentMesh->SetStaticMesh(nullptr);
	EquipmentMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	EquipmentMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	//BoxComponent->bHiddenInGame = false; //test
	BoxComponent->SetBoxExtent(FVector(5, 5, 5));
	BoxComponent->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
	BoxComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	SetRelativeScale3D(FVector::OneVector * 1.0f);
}

void UEmptyHandsComponent::Play(const FString& NextSection)
{
}

void UEmptyHandsComponent::PlayCharge()
{
}

void UEmptyHandsComponent::CalcChargeState(bool bForceAttack)
{
}

