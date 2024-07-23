// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameUnit.h"
#include "GenericTeamAgentInterface.h"
#include "MyCharacter.generated.h"

class AEnemy;
class AInteractObject;
class UPlayerUI;
class UAIPerceptionStimuliSourceComponent;

UCLASS(BlueprintType)
class THIRDGAME_API AMyCharacter : public AGameUnit, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	bool IsLockOnMode() { return bLockOn; }
	void SetOverlapInteractObject(TObjectPtr<AInteractObject> InteractObject);
	void SetPlayerUI(UPlayerUI* UI) { PlayerUI = UI; }
	TObjectPtr<UPlayerUI> GetPlayerUI() { return PlayerUI; }
	FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

	virtual void CalcMoveSpeed() override;
	virtual void CheckShield() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FName GetHostileUnitTag() override;
	virtual bool IsAvailableBehavior() override;
	virtual void SetTargetCharacter(TObjectPtr<AGameUnit> Unit) override;
	virtual void Damaged(float Damage, AGameUnit* DamageCauser) override;
	virtual void Death() override;
	virtual void HitRecover() override;
	virtual void AttackEnd(EQUIP_HAND_TYPE HandType, float BlendOut) override;
	virtual void ChargeAttackEnd(EQUIP_HAND_TYPE HandType, bool bInitState = true) override;
	virtual void OnTargeted(bool bTargeted) override;
	virtual void OnStagger() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float TickDelta) override;
	virtual void PlayHitReaction(const FName& SectionName, float PlayRate = 1.0f) override;

public:
	UFUNCTION(BlueprintCallable)
	bool IsEquipmentContinue(EQUIP_HAND_TYPE HandType);

	UFUNCTION(BlueprintCallable)
	bool IsEquipmentContinueCharge(EQUIP_HAND_TYPE HandType);

	UFUNCTION(BlueprintCallable)
	void ComboAttack(EQUIP_HAND_TYPE HandType, const FString& NextSection);

	UFUNCTION(BlueprintCallable)
	void ComboChargeAttack(EQUIP_HAND_TYPE HandType);

	UFUNCTION(BlueprintCallable)
	void ParryEnd();

	virtual void EvadeEnd() override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void LookSideTarget(const FInputActionValue& Value);
	void LockOnMode();
	void OrientCamera();
	void Run(const FInputActionValue& Value);
	void WeaponR1_Start(const FInputActionValue& Value);
	void WeaponR1_Trigger(const FInputActionValue& Value);
	void WeaponR1_Complete(const FInputActionValue& Value);
	void WeaponR2_Start(const FInputActionValue& Value);
	void WeaponR2_Trigger(const FInputActionValue& Value);
	void WeaponR2_Canceled(const FInputActionValue& Value);
	void WeaponR2_Complete(const FInputActionValue& Value);
	void WeaponL1_Start(const FInputActionValue& Value);
	void WeaponL1_Trigger(const FInputActionValue& Value);
	void WeaponL1_Complete(const FInputActionValue& Value);
	void WeaponL2_Start(const FInputActionValue& Value);
	void WeaponL2_Trigger(const FInputActionValue& Value);
	void WeaponL2_Canceled(const FInputActionValue& Value);
	void WeaponL2_Complete(const FInputActionValue& Value);
	void WeaponAbility(const FInputActionValue& Value);
	void Evade(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);

	TArray<TObjectPtr<AEnemy>> FindInViewportEnemyList();
	TTuple<FVector, FVector> GetMovementDirection();
	void Move(FVector2D MovementVector);
	void ResetState();
	FName GetEvadeInputDirection();

private:
	bool bLockOn = false;
	bool bWeaponAbility = false;
	float OcclusionTime = 0.0f;

	FVector2D MoveInputValue = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> Camera = nullptr;

	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
	TObjectPtr<UInputAction> MoveAction = nullptr;
	TObjectPtr<UInputAction> LookAction = nullptr;
	TObjectPtr<UInputAction> LockOnAction = nullptr;
	TObjectPtr<UInputAction> OrientCameraAction = nullptr;
	TObjectPtr<UInputAction> RunAction = nullptr;
	TObjectPtr<UInputAction> WeaponR1Action = nullptr;
	TObjectPtr<UInputAction> WeaponR2Action = nullptr;
	TObjectPtr<UInputAction> WeaponL1Action = nullptr;
	TObjectPtr<UInputAction> WeaponL2Action = nullptr;
	TObjectPtr<UInputAction> WeaponAbilityAction = nullptr;
	TObjectPtr<UInputAction> EvadeAction = nullptr;
	TObjectPtr<UInputAction> DodgeAction = nullptr;
	TObjectPtr<UInputAction> InteractAction = nullptr;
	
	TObjectPtr<AInteractObject> CurrentInteractObject = nullptr;

	TObjectPtr<UPlayerUI> PlayerUI = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIStimuliSource = nullptr;
};
