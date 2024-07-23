// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindNextAroundPos.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UBTTask_FindNextAroundPos : public UBTTaskNode
{
	GENERATED_BODY()


public:
	UBTTask_FindNextAroundPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
