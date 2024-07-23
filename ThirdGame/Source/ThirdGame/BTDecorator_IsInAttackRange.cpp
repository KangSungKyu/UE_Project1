// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "GameUnit.h"
#include "MyCharacter.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return false;
	}

	AMyCharacter* Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::TargetCharacterKey));

	if (Target == nullptr)
	{
		return false;
	}

	bResult = ControllingPawn->IsAvailableBehavior() && Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetAttackRange();

	//UE_LOG(LogTemp, Log, TEXT("%.2f, %.2f"), Target->GetDistanceTo(ControllingPawn), ControllingPawn->GetAttackRange());

	return bResult;
}
