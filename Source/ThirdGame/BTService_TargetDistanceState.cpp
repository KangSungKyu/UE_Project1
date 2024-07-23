// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TargetDistanceState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.h"
#include "GameUnit.h"
#include "MyCharacter.h"
#include "Enemy.h"

UBTService_TargetDistanceState::UBTService_TargetDistanceState()
{
	NodeName = TEXT("TargetDistanceState");
	Interval = 1.0f;
}

void UBTService_TargetDistanceState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return;
	}

	AEnemy* AIPawn = Cast<AEnemy>(ControllingPawn);
	float DistRatio = 1.0f;
	float TargetDist = 0.0f;
	float Range = 0.0f;

	if (AIPawn->GetTargetCharacter() != nullptr)
	{
		TargetDist = AIPawn->GetDistanceTo(AIPawn->GetTargetCharacter());
		Range = AIPawn->GetForwardSightRange();
		DistRatio = TargetDist / Range;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(AEnemyAIController::DistanceRatioKey, DistRatio);

	//UE_LOG(LogTemp, Log, TEXT("Dist %.2f, Range %.2f, DistRatio %.2f"), TargetDist, Range, DistRatio);
}
