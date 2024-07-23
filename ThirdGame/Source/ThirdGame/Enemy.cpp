// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/Actor.h"
#include "EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Widget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacter.h"
#include "PlayerUI.h"
#include "StatusProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "CharacterStatusComponent.h"
#include "EquipmentComponent.h"
#include "GameUnitInitializer.h"
#include "BrainComponent.h"
#include "EnemyRegion.h"
#include "NavigationSystem.h"

AEnemy::AEnemy()
{
	UnitType = UNIT_TYPE::ENEMY;

	bInViewport = false;
	bBossHUD = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bOnShield = false;
	bHitReaction = false;
	bForceWaitState = false;
	MaxWalkSpeed = 400;
	MaxRunSpeed = 650;
	MaxAcceleration = 550;
	MaxGuardWalkSpeed = 75;
	CurrentSavedSpeed = MaxWalkSpeed;
	RunCost = 5;
	MovementState = MOVEMENT_STATE::WALK;
	CharacterState = CHARACTER_STATE::ALIVE;
	UnitTag = TEXT("Game.Unit.Enemy");
	TeamId = 2;

	OcclusionTime = 0.0f;

	//GetCapsuleComponent()->bHiddenInGame = false; //test

	GetCharacterMovement()->RotationRate = FRotator(0, 320, 0);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->MaxStepHeight = 60;
	GetCharacterMovement()->SetWalkableFloorAngle(30);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	StatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>(TEXT("CharacterStatus"));
	HpBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBarWidget"));

	static ConstructorHelpers::FClassFinder<UUserWidget> TempHpBar(TEXT("Class'Game/Widgets/WBP_StatusProgressBar.WBP_StatusProgressBar_C'"));

	HpBarWidget->SetWidgetClass(TempHpBar.Class);
	HpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HpBarWidget->SetDrawSize(FVector2D(150, 20));
	HpBarWidget->SetRelativeLocation(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 1.825f + 10));
	HpBarWidget->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	HpBarWidget->SetupAttachment(GetCapsuleComponent());

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GameUnitInitializer::Init(this);

	StatusComponent->Delegate_ChangeHP.AddUObject(this, &AEnemy::OnChangedEnemyHP);
	StatusComponent->Delegate_RecoverPoise.AddUObject(this, &AEnemy::OnRecoverPoise);
	StatusComponent->Delegate_Stagger.AddUObject(this, &AEnemy::OnStagger);
	StatusComponent->SetStatus(SizeType, Stat);

	UStatusProgressBar* HpBar = Cast<UStatusProgressBar>(HpBarWidget->GetWidget());

	if (HpBar != nullptr)
	{
		HpBar->SetProgressBarFillColor(FLinearColor(1.0f, 0.1f, 0.1f));
	}

	SetVisibleHPBar(false);

	if (PatrolRegion != nullptr)
	{
		PatrolRegion->LinkEnemy(this);
	}
}

void AEnemy::Tick(float TickDelta)
{
	Super::Tick(TickDelta);

	if (!IsAvailableBehavior())
	{
		return;
	}

	bool bIsVisibleHPBar = HpBarWidget != nullptr && HpBarWidget->IsWidgetVisible();

	if (bIsVisibleHPBar && IsOcclusionTo(GetActorLocation(), TargetCharacter))
	{
		OcclusionTime += TickDelta;

		if (OcclusionTime >= 2.0f)
		{
			if (TargetCharacter != nullptr && TargetCharacter->GetTargetCharacter() == this)
			{
				TargetCharacter->SetTargetCharacter(nullptr);
			}

			SetVisibleHPBar(false);
			OcclusionTime = 0.0f;
		}
	}
	else
	{
		OcclusionTime = 0;
	}
}

void AEnemy::SetTargetCharacter(TObjectPtr<AGameUnit> Unit)
{
	if (CharacterState == CHARACTER_STATE::DEATH)
	{
		return;
	}

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->OnTargeted(false);
	}

	TargetCharacter = Unit;

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->OnTargeted(true);
	}

	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::TargetCharacterKey, Unit);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::GoHomeKey, false);
	}
}

void AEnemy::CalcMoveSpeed()
{
	bool bRunMode = MovementState == MOVEMENT_STATE::RUN;
	float NewSpeed = bRunMode ? MaxRunSpeed : MaxWalkSpeed;

	if (bOnShield)
	{
		if (GetShieldState() == 0)
		{
			NewSpeed = MaxGuardWalkSpeed;
		}
		else if (GetShieldState() == 1)
		{
			NewSpeed = 0;
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
		}
	}

	CurrentSavedSpeed = GetCharacterMovement()->MaxWalkSpeed;

	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	GetCharacterMovement()->MaxAcceleration = NewSpeed;
}

void AEnemy::CheckShield()
{
	bool bCheckR = false;
	bool bCheckL = false;

	if (EquipmentR != nullptr)
	{
		bCheckR = EquipmentR->CheckShield();
	}

	if (EquipmentL != nullptr)
	{
		bCheckL = EquipmentL->CheckShield();
	}

	bOnShield = bCheckR || bCheckL;
}

bool AEnemy::GetNextPatrolLocation(FVector& NextLoc)
{
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());
	float Radius = ForwardSightRange;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FVector Origin = EnemyController->GetBlackboardComponent()->GetValueAsVector(AEnemyAIController::CurrentPosKey);
	FNavLocation NextPatrol;
	bool bResult = false;

	NextLoc = FVector::ZeroVector;

	if (PatrolType == PATROL_TYPE::NONE)
	{
		NextLoc = Origin;
		bResult = true;
	}
	else if (PatrolType == PATROL_TYPE::RANDOM || PatrolType == PATROL_TYPE::REGION)
	{
		if (PatrolRegion != nullptr)
		{
			Origin = PatrolRegion->GetActorLocation();
			Radius = PatrolRegion->GetRegionRadius();
		}

		if (NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, NextPatrol))
		{
			NextLoc = NextPatrol.Location;
			bResult = true;
		}
	}
	else if (PatrolType == PATROL_TYPE::TWO_POINT)
	{

	}

	return bResult;
}

void AEnemy::GoHome()
{
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		bool bOrigin = EnemyController->GetBlackboardComponent()->GetValueAsBool(AEnemyAIController::GoHomeKey);

		if (!bOrigin)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::GoHomeKey, true);
			SetTargetCharacter(nullptr);
		}
	}
}

void AEnemy::ArriveHome()
{
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::GoHomeKey, false);
		SetVisibleHPBar(false);
	}
}

void AEnemy::Run(bool bRunMode)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	MovementState = StatusComponent->IsEnoughStamina(RunCost) && bRunMode ? MOVEMENT_STATE::RUN : MOVEMENT_STATE::WALK;

	CalcMoveSpeed();
}

void AEnemy::Evade()
{
	if (!IsAvailableBehavior() || !StatusComponent->IsEnoughStamina(EvadeCost))
	{
		return;
	}

	if (EvadeState == 0)
	{
		EvadeState = 2;

		StatusComponent->AddStamina(-EvadeCost);

		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimMontageEvade != nullptr)
		{
			AnimInstance->Montage_Play(AnimMontageEvade, 1.0f);
			AnimInstance->Montage_JumpToSection(TEXT("Evade"), AnimMontageEvade);
		}
	}
}

void AEnemy::EvadeEnd()
{
	EvadeState = 0;
}

FName AEnemy::GetHostileUnitTag()
{
	return TEXT("Game.Unit.Ally");
}

bool AEnemy::IsAvailableBehavior()
{
	bool bAlive = CharacterState == CHARACTER_STATE::ALIVE;
	bool bParryAction = GetShieldState() == 1;
	bool bAvailable = bAlive && !bExecutionMotion && !bHitReaction && EvadeState == 0;

	if (bOnShield && bParryAction)
	{
		bAvailable = false;
	}

	if (bForceWaitState)
	{
		bAvailable = false;
	}

	return bAvailable;
}

void AEnemy::PlayHitReaction(const FName& SectionName, float PlayRate)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontageHit->GetSectionIndex(SectionName) != INDEX_NONE)
	{
		AnimInstance->Montage_Play(AnimMontageHit, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontageHit);
	}
}

void AEnemy::SetVisibleHPBar(bool bValue)
{
	ESlateVisibility Value = ESlateVisibility::Hidden;
	TObjectPtr<UStatusProgressBar> HpBar = Cast<UStatusProgressBar>(HpBarWidget->GetWidget());

	if (CharacterState == CHARACTER_STATE::ALIVE && bValue && !bBossHUD)
	{
		Value = ESlateVisibility::Visible;
	}

	if (HpBar != nullptr)
	{
		HpBar->SetVisibility(Value);
	}
}

void AEnemy::OnChangedEnemyHP(float HP, float MaxHP)
{
	TObjectPtr<UStatusProgressBar> HpBar = Cast<UStatusProgressBar>(HpBarWidget->GetWidget());

	if (HpBar != nullptr)
	{
		HpBar->SetPercent(HP, MaxHP);
	}
}

void AEnemy::OnRecoverPoise()
{
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		EnemyController->GetBrainComponent()->ResumeLogic(TEXT(""));
		//EnemyController->GetBrainComponent()->RestartLogic();
	}
}

void AEnemy::Damaged(float Damage, AGameUnit* DamageCauser)
{
	if (CharacterState == CHARACTER_STATE::DEATH || Damage <= 0)
	{
		return;
	}

	FName SectionName = "";
	float PlayRate = 1.0f;
	bool bDamage = StatusComponent->Damaged(Damage, DamageCauser);
	bool bForwardHit = MyGameConfig::IsForwardHitDirection(DamageCauser, this);
	HIT_DIRECTION_TYPE HitDirection = MyGameConfig::GetHitDirectionType(DamageCauser, this);
	FCharacterStatus Status = GetStatus();

	if (bDamage)
	{
		if (Status.Hp <= 0)
		{
			Death();
		}
		else if (Status.Hp > 0 && bHitReaction)
		{
			if (bForwardHit)
			{
				SectionName = TEXT("FH");
			}
			else if (HitDirection == HIT_DIRECTION_TYPE::LEFT || HitDirection == HIT_DIRECTION_TYPE::FORWARD_LEFT)
			{
				SectionName = TEXT("LH");
			}
			else if (HitDirection == HIT_DIRECTION_TYPE::RIGHT || HitDirection == HIT_DIRECTION_TYPE::FORWARD_RIGHT)
			{
				SectionName = TEXT("RH");
			}
			else if (!bForwardHit)
			{
				SectionName = TEXT("BH");
			}
		}
	}
	else
	{
		if (Status.Stamina > 0)
		{
			//guard reaction
			SectionName = "GH";
		}
	}

	if (Status.PoisePoint <= 0)
	{
		//guard break, stagger
		SectionName = "GB";
		PlayRate = 0.25f;
	}

	//UE_LOG(LogTemp, Log, TEXT("character state, state %d, hp %.2f, stamina %.2f, dmg %.2f, poise %.2f, anim %s"), CharacterState, Status.Hp, Status.Stamina, Damage, Status.PoisePoint, *SectionName.ToString());

	if (SectionName != "" && !bExecutionMotion)
	{
		AttackEnd(EQUIP_HAND_TYPE::RIGHT_HAND, 1.0f);
		AttackEnd(EQUIP_HAND_TYPE::LEFT_HAND, 1.0f);
		PlayHitReaction(SectionName, PlayRate);
	}

	if (GetForceWaitState())
	{
		SetForceWaitState(false);
	}

	SetVisibleHPBar(true);
	StatusComponent->ChargePoisePoint();
	SetTargetCharacter(DamageCauser);

	if (SizeType == UNIT_SIZE_TYPE::BOSS && DamageCauser->GetUnitCode() == UNIT_CODE::CHARACTER)
	{
		TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(DamageCauser);

		if (Player != nullptr)
		{
			Player->GetPlayerUI()->SetBossHUD(this);
		}
	}
}

void AEnemy::Death()
{
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(Controller);

	SetVisibleHPBar(false);

	CharacterState = CHARACTER_STATE::DEATH;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->SetTargetCharacter(nullptr);
	}

	if (EnemyController != nullptr)
	{
		EnemyController->GetBrainComponent()->StopLogic(TEXT(""));
	}
}

void AEnemy::HitRecover()
{
	FCharacterStatus Status = GetStatus();

	SetHitReaction(false);

	if (Status.Hp > 0)
	{
		CharacterState = CHARACTER_STATE::ALIVE;
	}
	else
	{
		Death();
	}
}

void AEnemy::AttackEnd(EQUIP_HAND_TYPE HandType, float BlendOut)
{
	if (HandType == EQUIP_HAND_TYPE::RIGHT_HAND && EquipmentR != nullptr)
	{
		EquipmentR->End(BlendOut);
	}
	else if (HandType == EQUIP_HAND_TYPE::LEFT_HAND && EquipmentL != nullptr)
	{
		EquipmentL->End(BlendOut);
	}
}

void AEnemy::ChargeAttackEnd(EQUIP_HAND_TYPE HandType, bool bInitState)
{
	if (HandType == EQUIP_HAND_TYPE::RIGHT_HAND && EquipmentR != nullptr)
	{
		EquipmentR->ChargeEnd(bInitState);
	}
	else if (HandType == EQUIP_HAND_TYPE::LEFT_HAND && EquipmentL != nullptr)
	{
		EquipmentL->ChargeEnd(bInitState);
	}
}

void AEnemy::OnTargeted(bool bTargeted)
{
	SetVisibleHPBar(bTargeted);
}

void AEnemy::OnStagger()
{
	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		EnemyController->GetBrainComponent()->PauseLogic(TEXT(""));
		//EnemyController->GetBrainComponent()->StopLogic(TEXT(""));
	}

	SetHitReaction(true);
	AttackEnd(EQUIP_HAND_TYPE::RIGHT_HAND, 1.0f);
	AttackEnd(EQUIP_HAND_TYPE::LEFT_HAND, 1.0f);
	PlayHitReaction(TEXT("GB"), 0.25f);
}

void AEnemy::SetForceWaitState(bool bValue)
{
	Super::SetForceWaitState(bValue);

	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		if (bValue)
		{
			EnemyController->GetBrainComponent()->PauseLogic(TEXT(""));
		}
		else
		{
			EnemyController->GetBrainComponent()->ResumeLogic(TEXT(""));
		}

		EnemyController->GetBlackboardComponent()->SetValueAsBool(AEnemyAIController::ForceWaitStateKey, bValue);
	}
}

void AEnemy::ExecutionStart()
{
	Super::ExecutionStart();

	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		EnemyController->GetBrainComponent()->PauseLogic(TEXT(""));
		//EnemyController->GetBrainComponent()->StopLogic(TEXT(""));
	}
}

void AEnemy::ExecutionEnd()
{
	Super::ExecutionEnd();

	TObjectPtr<AEnemyAIController> EnemyController = Cast<AEnemyAIController>(GetController());

	if (EnemyController != nullptr)
	{
		EnemyController->GetBrainComponent()->ResumeLogic(TEXT(""));
		//EnemyController->GetBrainComponent()->RestartLogic();
	}
}