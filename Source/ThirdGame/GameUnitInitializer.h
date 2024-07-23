// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameConfig.h"

class AGameUnit;

/**
 * 
 */
class THIRDGAME_API GameUnitInitializer
{
public:
	static void Init(TObjectPtr<AGameUnit> Unit, UNIT_CODE Code = UNIT_CODE::UNIT_CODE_END);
	static TObjectPtr<UBehaviorTree> GetBehaviourTree(TObjectPtr<AGameUnit> Unit);
	static TObjectPtr<UBlackboardData> GetBlackboard(TObjectPtr<AGameUnit> Unit);

private:
	static void Init_Character(TObjectPtr<AGameUnit> Unit);
	static void Init_NormalWarrior(TObjectPtr<AGameUnit> Unit);
	static void Init_NormalHandThrower(TObjectPtr<AGameUnit> Unit);
	static void Init_EliteKatana(TObjectPtr<AGameUnit> Unit);
	static void Init_BossGreatSwrod(TObjectPtr<AGameUnit> Unit);
};
