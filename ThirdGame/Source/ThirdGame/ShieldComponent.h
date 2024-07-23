// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentComponent.h"
#include "ShieldComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API UShieldComponent : public UEquipmentComponent
{
	GENERATED_BODY()

public:
	virtual void PlayStart() override;
	virtual void PlayTrigger() override;
	virtual void PlayCompleted() override;
	virtual void PlayChargeStart() override;
	virtual void ChargeEnd(bool bInitState = true) override;

	virtual void End(float BlendOut) override;

	virtual void PlayCombo(const FString& NextSection) override;
	virtual void PlayChargeCombo() override;

protected:
	UShieldComponent();

protected:
	virtual void InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh) override;
	virtual void Play(const FString& NextSection = "") override;
	virtual void PlayCharge() override;
	virtual void CalcChargeState(bool bForceAttack = false) override;
};
