// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindNextAroundPos.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Enemy.h"



UBTTask_FindNextAroundPos::UBTTask_FindNextAroundPos()
{
	NodeName = TEXT("FindNextAroundPos");
}

EBTNodeResult::Type UBTTask_FindNextAroundPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<AEnemy> ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector NextAroundPos = FVector::ZeroVector;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(ControllingPawn->GetController());
	FVector Origin = EnemyController->GetBlackboardComponent()->GetValueAsVector(AEnemyAIController::CurrentPosKey);
	FVector ToTarget = Origin;
	float Radius = ControllingPawn->GetAttackRange() * 0.5f;
	float TargetRadius = ControllingPawn->GetAttackRange();
	FNavLocation FromMe;
	FNavLocation FromTarget;

	if (ControllingPawn->GetTargetCharacter() != nullptr)
	{
		ToTarget = ControllingPawn->GetTargetCharacter()->GetActorLocation();
	}

	bool bFromMe = NavSystem->GetRandomPointInNavigableRadius(ToTarget, Radius, FromMe);
	bool bFromTarget = NavSystem->GetRandomPointInNavigableRadius(ToTarget, TargetRadius, FromTarget);

	if (bFromMe && bFromTarget)
	{
		float Ratio = FMath::RandRange(0.5f, 1.0f);
		NextAroundPos = FMath::Lerp(FromMe.Location, FromTarget.Location, Ratio);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::AroundPosKey, NextAroundPos);
	}
	else if (bFromMe || bFromTarget)
	{
		NextAroundPos = bFromMe ? FromMe.Location : FromTarget.Location;
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::AroundPosKey, NextAroundPos);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::AroundPosKey, ToTarget);
	}

	return EBTNodeResult::Succeeded;
}
