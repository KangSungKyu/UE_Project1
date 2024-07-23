// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TargetDistanceState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UBTService_TargetDistanceState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_TargetDistanceState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
