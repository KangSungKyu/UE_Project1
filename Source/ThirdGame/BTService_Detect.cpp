// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.h"
#include "GameUnit.h"
#include "MyCharacter.h"
#include "Enemy.h"
#include "CharacterStatusComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	/*
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return;
	}

	AEnemy* AIPawn = Cast<AEnemy>(ControllingPawn);
	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	FVector AIForwardVector = AIPawn->GetActorForwardVector();
	float DetectRange = AIPawn->GetForwardSightRange();
	float DetectAngle = AIPawn->GetForwardSightAngle();
	float HalfAngle = DetectAngle * 0.5f;
	FVector ForwardEnd = Center + AIForwardVector * DetectRange;
	FVector ForwardNorm = ForwardEnd;
	FVector SightEnd1 = Center + AIForwardVector.RotateAngleAxis(-HalfAngle, FVector::UpVector) * DetectRange;
	FVector SightEnd2 = Center + AIForwardVector.RotateAngleAxis(HalfAngle, FVector::UpVector) * DetectRange;

	if (World == nullptr)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel
	(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel12,
		FCollisionShape::MakeSphere(DetectRange),
		CollisionQueryParam
	);

	//DrawDebugSphere(World, Center, DetectRange, 16, FColor::Red, false, 0.4f);
	//DrawDebugDirectionalArrow(World, Center, ForwardEnd, 120, FColor::Red, false, 1.0f);
	//DrawDebugLine(World, Center, SightEnd1, FColor::Red, false, 1.0f);
	//DrawDebugLine(World, Center, SightEnd2, FColor::Red, false, 1.0f);

	if (bResult)
	{
		for (FOverlapResult const& OverlapResult : OverlapResults)
		{
			AGameUnit* Unit = Cast<AGameUnit>(OverlapResult.GetActor());

			if (Unit && Unit->GetController()->IsPlayerController())
			{
				bool InSight = false;
				FVector UnitPos = Unit->GetActorLocation();
				FVector Dir = (UnitPos - Center).GetUnsafeNormal();
				FVector Cross = FVector::CrossProduct(AIForwardVector, Dir);
				float Angle = FMath::RadiansToDegrees(FMath::Acos(AIForwardVector.CosineAngle2D(Dir)));

				if (Cross.Z > 0)
				{
					InSight = Angle <= HalfAngle;
				}
				else
				{
					InSight = -Angle >= -HalfAngle;
				}

				if (InSight)
				{
					AIPawn->GetStatusComponent()->ChargePoisePoint();
					AIPawn->SetTargetCharacter(Unit);

					//DrawDebugSphere(World, Center, DetectRange, 16, FColor::Green, false, 0.4f);
					//DrawDebugPoint(World, UnitPos, 10.0f, FColor::Blue, false, 0.4f);
					//DrawDebugDirectionalArrow(World, Center, UnitPos, 120, FColor::Blue, false, 0.4f);

					return;
				}
			}
		}
	}
	//*/
}
