// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "GameUnitInitializer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameUnit.h"
#include "Enemy.h"
#include "CharacterStatusComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"


const FName AEnemyAIController::CurrentPosKey(TEXT("CurrentPos"));
const FName AEnemyAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AEnemyAIController::TargetCharacterKey(TEXT("TargetCharacter"));
const FName AEnemyAIController::ForceWaitStateKey(TEXT("ForceWaitState"));
const FName AEnemyAIController::SpawnPosKey(TEXT("SpawnPos"));
const FName AEnemyAIController::GoHomeKey(TEXT("GoHome"));
const FName AEnemyAIController::DistanceRatioKey(TEXT("DistanceRatio"));
const FName AEnemyAIController::AroundPosKey(TEXT("AroundPos"));

AEnemyAIController::AEnemyAIController()
{
	BTAsset = nullptr;
	BBAsset = nullptr;

	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI_Sight"));

	SetPerceptionComponent(*Perception);

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemyAIController::StartBehaviorTree()
{
	if (BTAsset != nullptr)
	{
		RunBehaviorTree(BTAsset);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TObjectPtr<AGameUnit> Unit = Cast<AGameUnit>(GetCharacter());

	if (Unit != nullptr)
	{
		SetBehaviourTree(GameUnitInitializer::GetBehaviourTree(Unit));
		SetBlackboard(GameUnitInitializer::GetBlackboard(Unit));
		SetGenericTeamId(Unit->GetTeamId());
	}

	UBlackboardComponent* BlackboardComponent = Blackboard;

	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		Blackboard->SetValueAsVector(CurrentPosKey, InPawn->GetActorLocation());
		Blackboard->SetValueAsVector(SpawnPosKey, InPawn->GetActorLocation());
		Blackboard->SetValueAsBool(GoHomeKey, false);
		Blackboard->SetValueAsFloat(DistanceRatioKey, 1.0f);
	}

	StartBehaviorTree();
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	TWeakObjectPtr<AEnemy> OwnerCharacter = Cast<AEnemy>(GetCharacter());

	if (OwnerCharacter != nullptr)
	{
		SightConfig->SightRadius = OwnerCharacter->GetForwardSightRange(); // �ð� �ݰ�
		SightConfig->LoseSightRadius = OwnerCharacter->GetForwardSightRange() + 400; // �ð� ��� �ݰ�
		SightConfig->PeripheralVisionAngleDegrees = OwnerCharacter->GetForwardSightAngle(); // �ֺ� �þ߰�
		SightConfig->DetectionByAffiliation.bDetectEnemies = true; // �ҼӺ� Ž�� ��
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // �ҼӺ� Ž�� ��
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // �ҼӺ� Ž�� �߸�
		SightConfig->SetMaxAge(5.0f); //�ڱ��� ������ ������ �ð� (0�̸� ��������)

		UE_LOG(LogTemp, Log, TEXT("setup perception"));
	}

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus)
{
	TWeakObjectPtr<AEnemy> OwnerCharacter = Cast<AEnemy>(GetCharacter());
	TObjectPtr<AGameUnit> Unit = Cast<AGameUnit>(Actor);

	if (Unit != nullptr && Unit->GetUnitTag() == OwnerCharacter->GetHostileUnitTag())
	{
		OwnerCharacter->GetStatusComponent()->ChargePoisePoint();
		OwnerCharacter->SetTargetCharacter(Unit);
	}
}

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& Actors)
{
	TArray<AActor*> Result;

	Perception->GetCurrentlyPerceivedActors(nullptr, Result);

	if (Result.Num() > 0)
	{
		TWeakObjectPtr<AEnemy> OwnerCharacter = Cast<AEnemy>(GetCharacter());

		for (int i = 0; i < Result.Num(); ++i)
		{
			TObjectPtr<AGameUnit> Unit = Cast<AGameUnit>(Result[i]);

			if (Unit != nullptr && Unit->GetUnitTag() == OwnerCharacter->GetHostileUnitTag())
			{
				OwnerCharacter->GetStatusComponent()->ChargePoisePoint();
				OwnerCharacter->SetTargetCharacter(Unit);
				break;
			}
		}
	}
}
