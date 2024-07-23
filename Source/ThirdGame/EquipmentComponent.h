// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameConfig.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "EquipmentComponent.generated.h"

class AGameUnit;
class UCharacterStatusComponent;

UCLASS( Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDGAME_API UEquipmentComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:
	static TObjectPtr<UEquipmentComponent> CreateEquipment(TObjectPtr<AGameUnit> Owner, TObjectPtr<UMeshComponent> OwnerMesh, EQUIP_HAND_TYPE HandType, const FName& ItemName, TSubclassOf<UEquipmentComponent> SubClass);

public:
	EQUIPMENT_TYPE GetType() { return EquipmentType; }
	EQUIP_HAND_TYPE GetHandType() { return EquipHandType; }
	bool IsPlay() { return bPlay; }
	bool IsContinue() { return bContinue; }
	bool IsPlayCharge() { return bPlayCharge; }
	bool IsContinueCharge() { return bContinueCharge; }
	uint8 GetState() { return State; }
	uint8 GetStateEnd() { return StateEnd; }
	uint8 GetChargeState() { return ChargeState; }
	uint8 GetChargeStateEnd() { return ChargeStateEnd; }
	TObjectPtr<AGameUnit> GetOwnerCharacter() { return OwnerCharacter; }
	bool CheckShield();
	bool IsAvailableBehavior(TObjectPtr<UCharacterStatusComponent> StatusComponent, bool bNormal);
	void SetBoxCollision(bool bValue);
	bool CheckSweepCollision(FHitResult& HitResult);

	virtual void PlayStart();
	virtual void PlayTrigger();
	virtual void PlayCompleted();
	virtual void PlayChargeStart();
	virtual void PlayChargeTrigger();
	virtual void PlayChargeCanceled();
	virtual void PlayChargeCompleted();

	virtual void PlayCombo(const FString& NextSection) PURE_VIRTUAL(UEquipmentComponent::PlayCombo, );
	virtual void PlayChargeCombo() PURE_VIRTUAL(UEquipmentComponent::PlayChargeCombo, );
	virtual void End(float BlendOut) PURE_VIRTUAL(UEquipmentComponent::End, );
	virtual void ChargeEnd(bool bInitState = true) PURE_VIRTUAL(UEquipmentComponent::ChargeEnd, );

protected:
	UEquipmentComponent();

	void CreateMeshAndBox();
	FString GetEquipSocketName();

	virtual void InitEquipment(TObjectPtr<UMeshComponent> OwnerMesh) PURE_VIRTUAL(UEquipmentComponent::InitEquipment, );
	virtual void Play(const FString& NextSection = "") PURE_VIRTUAL(UEquipmentComponent::Play, );
	virtual void PlayCharge() PURE_VIRTUAL(UEquipmentComponent::PlayCharge, );
	virtual void CalcChargeState(bool bForceAttack = false) PURE_VIRTUAL(UEquipmentComponent::CalcChargeState, );

	virtual bool IsAvailable();
	virtual bool IsAvailableCharge();


public:
	FStatusDelegate_UseStamina Delegate_UseStamina;

protected:
	EQUIPMENT_TYPE EquipmentType = EQUIPMENT_TYPE::NONE;
	EQUIP_HAND_TYPE EquipHandType = EQUIP_HAND_TYPE::RIGHT_HAND;
	bool bPlay = false;
	bool bPlayCharge = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bContinue = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bContinueCharge = false;

	uint8 State = 0;
	uint8 ChargeState = 0;
	uint8 StateEnd = 0;
	uint8 ChargeCycleCount = 0;
	uint8 ChargeStateEnd = 0;
	float NormalCost = 0;
	float ChargeCost = 0;
	FName EquipmentName = "";
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> EquipmentMesh = nullptr;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent = nullptr;
	TObjectPtr<UAnimMontage> NormalMontage = nullptr;
	TObjectPtr<UAnimMontage> ChargeMontage = nullptr;
	TObjectPtr<AGameUnit> OwnerCharacter = nullptr;
};
