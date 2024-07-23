// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentComponent.h"
#include "EmptyHandsComponent.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UEmptyHandsComponent : public UEquipmentComponent
{
	GENERATED_BODY()
public:

	virtual void PlayCombo(const FString& NextSection) override;
	virtual void PlayChargeCombo() override;
	virtual void End(float BlendOut) override;
	virtual void ChargeEnd(bool bInitState = true) override;

protected:
	UEmptyHandsComponent();

protected:
	virtual void InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh) override;
	virtual void Play(const FString& NextSection = "") override;
	virtual void PlayCharge() override;
	virtual void CalcChargeState(bool bForceAttack = false) override;
};
