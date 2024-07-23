// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_NormalAttack.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "EquipmentComponent.h"

UBTTask_NormalAttack::UBTTask_NormalAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_NormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEquipmentComponent* EquipmentR = ControllingPawn->GetEquipmentR();

	if (ControllingPawn == nullptr || !ControllingPawn->IsAvailableBehavior() || !EquipmentR->IsAvailableBehavior(ControllingPawn->GetStatusComponent(), true))
	{
		return EBTNodeResult::Failed;
	}


	if (EquipmentR != nullptr && !EquipmentR->IsPlay())
	{
		EquipmentR->PlayStart();
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_NormalAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return;
	}

	if (!ControllingPawn->GetEquipmentR()->IsPlay())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

