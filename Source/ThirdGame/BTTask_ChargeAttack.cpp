// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChargeAttack.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "EquipmentComponent.h"

UBTTask_ChargeAttack::UBTTask_ChargeAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UEquipmentComponent* EquipmentR = ControllingPawn->GetEquipmentR();

	if (ControllingPawn == nullptr || !ControllingPawn->IsAvailableBehavior() || !EquipmentR->IsAvailableBehavior(ControllingPawn->GetStatusComponent(), true))
	{
		return EBTNodeResult::Failed;
	}


	if (EquipmentR != nullptr && !EquipmentR->IsPlayCharge())
	{
		EquipmentR->PlayChargeStart();
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return;
	}

	if (!ControllingPawn->GetEquipmentR()->IsPlayCharge())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}



