// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUnit.h"
#include "Enemy.generated.h"

class USphereComponent;
class UWidgetComponent;
class AEnemyRegion;
/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API AEnemy : public AGameUnit
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	bool IsInViewport() { return bInViewport; }
	void SetBossHUD(bool bValue) { bBossHUD = bValue; }
	float GetForwardSightAngle() { return ForwardSightAngle; }
	float GetForwardSightRange() { return ForwardSightRange; }
	float GetAttackRange() { return AttackRange; }
	float GetAcceptableRangeRatio() { return AcceptableRangeRatio; }
	PATROL_TYPE GetPatrolType() { return PatrolType; }
	bool GetNextPatrolLocation(FVector& NextLoc);
	void GoHome();

	UFUNCTION(BlueprintCallable)
	void ArriveHome();

	UFUNCTION(BlueprintCallable)
	void Run(bool bRunMode);

	UFUNCTION(BlueprintCallable)
	void Evade();

	void SetVisibleHPBar(bool bValue);

	virtual void EvadeEnd() override;

	virtual FName GetHostileUnitTag() override;
	virtual bool IsAvailableBehavior() override;
	virtual void SetTargetCharacter(TObjectPtr<AGameUnit> Unit) override;
	virtual void CalcMoveSpeed() override;
	virtual void CheckShield() override;
	virtual void Damaged(float Damage, AGameUnit* DamageCauser) override;
	virtual void Death() override;
	virtual void HitRecover() override;
	virtual void AttackEnd(EQUIP_HAND_TYPE HandType, float BlendOut) override;
	virtual void ChargeAttackEnd(EQUIP_HAND_TYPE HandType, bool bInitState = true) override;
	virtual void OnTargeted(bool bTargeted) override;
	virtual void OnStagger() override;

	virtual void SetForceWaitState(bool bValue) override;

	virtual void ExecutionStart() override;
	virtual void ExecutionEnd() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float TickDelta) override;
	virtual void PlayHitReaction(const FName& SectionName, float PlayRate = 1.0f) override;

private:
	void OnChangedEnemyHP(float HP, float MaxHP);
	void OnRecoverPoise();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bInViewport = false;

	bool bBossHUD = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardSightAngle = 120;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardSightRange = 120;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AttackRange = 150;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AcceptableRangeRatio = 1.0f;

	UPROPERTY(EditAnywhere)
	PATROL_TYPE PatrolType = PATROL_TYPE::NONE;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetComponent> HpBarWidget = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AEnemyRegion> PatrolRegion;

private:
	float OcclusionTime = 0.0f;
};
