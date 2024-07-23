// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameUnit.h"
#include "MyCharacter.h"
#include "Enemy.h"
#include "EnemyAIController.h"

UBTTask_MoveToAttackRange::UBTTask_MoveToAttackRange()
{
	NodeName = TEXT("MoveToAttackRange");
}

EBTNodeResult::Type UBTTask_MoveToAttackRange::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn != nullptr)
	{
		AcceptableRadius = ControllingPawn->GetAttackRange() * ControllingPawn->GetAcceptableRangeRatio();
	}

	return Super::PerformMoveTask(OwnerComp, NodeMemory);
}
