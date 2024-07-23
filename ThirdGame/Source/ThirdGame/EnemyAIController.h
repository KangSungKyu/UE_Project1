// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	void SetBehaviourTree(TObjectPtr<UBehaviorTree> BT) { BTAsset = BT; }
	void SetBlackboard(TObjectPtr<UBlackboardData> BB) { BBAsset = BB; }
	void StartBehaviorTree();

	virtual void OnPossess(APawn* InPawn) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus);
	
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& Actors);


public:
	static const FName CurrentPosKey;
	static const FName PatrolPosKey;
	static const FName TargetCharacterKey;
	static const FName ForceWaitStateKey;
	static const FName SpawnPosKey;
	static const FName GoHomeKey;
	static const FName DistanceRatioKey;
	static const FName AroundPosKey;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BTAsset = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlackboardData> BBAsset = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAIPerceptionComponent> Perception = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;
};
