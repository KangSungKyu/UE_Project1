// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "EnemyAIController.h"
#include "Enemy.h"
#include "MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AMyCharacter* Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::TargetCharacterKey));
	
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector LookVector = Target->GetActorLocation() - ControllingPawn->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), HomingTime));

	return EBTNodeResult::Succeeded;
}