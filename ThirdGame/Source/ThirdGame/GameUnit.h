// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameConfig.h"
#include "GameFramework/Character.h"
#include "GameUnit.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

class UEquipmentComponent;
class UCharacterStatusComponent;

struct FInputActionValue;

UCLASS(Abstract, BlueprintType)
class THIRDGAME_API AGameUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameUnit();

public:
	UFUNCTION(BlueprintCallable)
	FCharacterStatus GetStatus();

	UFUNCTION(BlueprintCallable)
	UCharacterStatusComponent* GetStatusComponent();

	UFUNCTION(BlueprintCallable)
	AGameUnit* GetTargetCharacter() { return TargetCharacter; }

	UFUNCTION(BlueprintCallable)
	UEquipmentComponent* GetEquipmentR() { return EquipmentR; }
	
	UFUNCTION(BlueprintCallable)
	UEquipmentComponent* GetEquipmentL() { return EquipmentL; }

	UFUNCTION(BlueprintCallable)
	uint8 GetShieldState();

	UFUNCTION(BlueprintCallable)
	bool IsPlayAttackAnyEquipment();

	UFUNCTION(BlueprintCallable)
	virtual void ExecutionStart();

	UFUNCTION(BlueprintCallable)
	virtual void ExecutionEnd();

	UFUNCTION()
	void SimpleNormalAttackFromEquipmentR();

	UFUNCTION()
	void SimpleRunAttackFromEquipmentR();

	UFUNCTION(BlueprintCallable)
	FVector GetLockOnSocketLocation();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd(EQUIP_HAND_TYPE HandType, float BlendOut) PURE_VIRTUAL(AGameUnit::AttackEnd, );

	UFUNCTION(BlueprintCallable)
	virtual void ChargeAttackEnd(EQUIP_HAND_TYPE HandType, bool bInitState = true) PURE_VIRTUAL(AGameUnit::ChargeAttackEnd, );

	UFUNCTION(BlueprintCallable)
	virtual void Damaged(float Damage, AGameUnit* DamageCauser) PURE_VIRTUAL(AGameUnit::Damaged, );

	UFUNCTION(BlueprintCallable)
	virtual void Death() PURE_VIRTUAL(AGameUnit::Death, );

	UFUNCTION(BlueprintCallable)
	virtual void HitRecover() PURE_VIRTUAL(AGameUnit::HitRecover, );

	UFUNCTION(BlueprintCallable)
	virtual void EvadeEnd() PURE_VIRTUAL(AGameUnit::EvadeEnd, );

public:
	void SetUnitCode(UNIT_CODE Code) { UnitCode = Code; }
	UNIT_CODE GetUnitCode() { return UnitCode; }
	UNIT_SIZE_TYPE GetUnitSizeType() { return SizeType; }
	CHARACTER_STATE GetCharacterState() { return CharacterState; }
	MOVEMENT_STATE GetMovementState() { return MovementState; }
	bool IsHitReaction() { return bHitReaction; }
	bool IsOnShield() { return bOnShield; }
	bool IsRecoveryStamina();
	void SetHitReaction(bool bValue) { bHitReaction = bValue; }
	void SetEquipmentR(TObjectPtr<UEquipmentComponent> Equipment) { EquipmentR = Equipment; }
	void SetEquipmentL(TObjectPtr<UEquipmentComponent> Equipment) { EquipmentL = Equipment; }
	FName GetUnitTag() { return UnitTag; }
	bool GetForceWaitState() { return bForceWaitState; }
	uint8 GetTeamId() { return TeamId; }
	
	
	void SetAnimMontageHit(const TObjectPtr<UAnimMontage>& Montage) { AnimMontageHit = Montage; }
	void SetAnimMontageExecute(const TObjectPtr<UAnimMontage>& Montage) { AnimMontageExecute = Montage; }
	void SetAnimMontageExecuted(const TObjectPtr<UAnimMontage>& Montage) { AnimMontageExecuted = Montage; }
	void SetAnimMontageEvade(const TObjectPtr<UAnimMontage>& Montage) { AnimMontageEvade = Montage; }

	void PlayExecute(const FName& SectionName, AGameUnit* Target);
	void PlayExecuted(const FName& SectionName);

	bool IsOcclusionTo(const FVector& StartLocation, const TObjectPtr<AGameUnit>& Unit);

	virtual FName GetHostileUnitTag() PURE_VIRTUAL(AGameUnit::GetHostileUnitTag, return TEXT(""););
	virtual bool IsAvailableBehavior() PURE_VIRTUAL(AGameUnit::IsAvailableBehavior, return false;);
	virtual void SetTargetCharacter(TObjectPtr<AGameUnit> Unit) PURE_VIRTUAL(AGameUnit::SetTargetCharacter);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void CalcMoveSpeed() PURE_VIRTUAL(AGameUnit::CalcMoveSpeed, );
	virtual void CheckShield() PURE_VIRTUAL(AGameUnit::CheckShield, );
	virtual void OnTargeted(bool bTargeted) PURE_VIRTUAL(AGameUnit::OnTargeted, );
	virtual void OnStagger() PURE_VIRTUAL(AGameUnit::OnStagger, );

	virtual void SetForceWaitState(bool bValue) { bForceWaitState = bValue; }

protected:
	virtual void PlayHitReaction(const FName& SectionName, float PlayRate = 1.0f) PURE_VIRTUAL(AGameUnit::PlayHitReaction, );
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UNIT_TYPE UnitType = UNIT_TYPE::NPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UNIT_SIZE_TYPE SizeType = UNIT_SIZE_TYPE::NORMAL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UNIT_CODE UnitCode = UNIT_CODE::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	CHARACTER_STATE CharacterState = CHARACTER_STATE::ALIVE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	MOVEMENT_STATE MovementState = MOVEMENT_STATE::WALK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bHitReaction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnShield = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bExecutionMotion = false;

	UPROPERTY(EditAnywhere)
	bool bForceWaitState = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int EvadeState = false;

	UPROPERTY(EditAnywhere)
	uint8 TeamId = 255;

	UPROPERTY(EditAnywhere)
	float MaxWalkSpeed = 0;

	UPROPERTY(EditAnywhere)
	float MaxRunSpeed = 0;

	UPROPERTY(EditAnywhere)
	float MaxAcceleration = 0;

	UPROPERTY(EditAnywhere)
	float MaxGuardWalkSpeed = 0;

	float CurrentSavedSpeed = 0;
	float RunCost = 0;
	float EvadeCost = 25;

	FName UnitTag = TEXT("");

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontageHit = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontageExecute = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontageExecuted = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AnimMontageEvade = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UEquipmentComponent> EquipmentR = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UEquipmentComponent> EquipmentL = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AGameUnit> TargetCharacter = nullptr;

	TSubclassOf<AActor> TargetCharacterClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FCharacterStat Stat;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCharacterStatusComponent> StatusComponent = nullptr;
};
