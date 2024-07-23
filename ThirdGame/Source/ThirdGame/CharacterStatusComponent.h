// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameConfig.h"
#include "Components/ActorComponent.h"
#include "CharacterStatusComponent.generated.h"

class AGameUnit;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType)
class THIRDGAME_API UCharacterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatusComponent();

	void SetStatus(UNIT_SIZE_TYPE SizeType, const FCharacterStat& Stat);
	FCharacterStatus GetStatus() { return Status; }

	bool Damaged(float Damage, TObjectPtr<AGameUnit> Causer);
	bool ExecutionDamaged(float Damage, TObjectPtr<AGameUnit> Causer);
	bool IsEnoughStamina(float Stamina);
	void ResetPoisePoint();
	void ChargePoisePoint();
	void SetPoisePoint(float PoisePoint);
	void SetPoiseTimer();

	UFUNCTION(BlueprintCallable)
	void AddStamina(float Stamina);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void PoiseTimer();
	void CalcPoisePoint(float HitPoint);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FStatusDelegate_ChangeHP Delegate_ChangeHP;
	FStatusDelegate_ChangeMaxHP Delegate_ChangeMaxHP;
	FStatusDelegate_ChangeStamina Delegate_ChangeStamina;
	FStatusDelegate_ChangeMaxStamina Delegate_ChangeMaxStamina;
	FStatusDelegate_RecoverPoise Delegate_RecoverPoise;
	FStatusDelegate_Stagger Delegate_Stagger;

private:
	float PoiseRecoverTime = 2.5f;

	TObjectPtr<AGameUnit> Owner = nullptr;
	FCharacterStatus Status;
	FTimerHandle PoiseTimerHandle;
};
