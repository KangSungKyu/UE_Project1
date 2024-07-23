// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUnitInitializer.h"
#include "Components/CapsuleComponent.h"
#include "GameUnit.h"
#include "MyCharacter.h"
#include "Enemy.h"
#include "EquipmentComponent.h"
#include "CharacterStatusComponent.h"
#include "EnemyAIController.h"
#include "OneHandSwordComponent.h"
#include "ShieldComponent.h"
#include "EmptyOneHandSwordComponent.h"
#include "EmptyHandThrowerComponent.h"
#include "ElitesKatanaComponent.h"
#include "BossyGreatSwordComponent.h"
#include "EmptyHandsComponent.h"


void GameUnitInitializer::Init(TObjectPtr<AGameUnit> Unit, UNIT_CODE Code)
{
	if (Code != UNIT_CODE::UNIT_CODE_END)
	{
		Unit->SetUnitCode(Code);
	}

	Unit->Tags.Add(Unit->GetUnitTag());

	switch (Unit->GetUnitCode())
	{
	case UNIT_CODE::CHARACTER:
		Init_Character(Unit);
		break;
	case UNIT_CODE::NORMAL_WARRIOR:
		Init_NormalWarrior(Unit);
		break;
	case UNIT_CODE::NORMAL_HANDTHROWER:
		Init_NormalHandThrower(Unit);
		break;
	case UNIT_CODE::ELITE_KATANA:
		Init_EliteKatana(Unit);
		break;
	case UNIT_CODE::BOSS_GREATSWORD:
		Init_BossGreatSwrod(Unit);
		break;
	}
}

TObjectPtr<UBehaviorTree> GameUnitInitializer::GetBehaviourTree(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<UBehaviorTree> Result = nullptr;
	FString Path = "";

	switch (Unit->GetUnitCode())
	{
	case UNIT_CODE::NORMAL_WARRIOR:
		Path = TEXT("BehaviorTree'/Game/Frameworks/EnemyAI/BT_NormalWarrior.BT_NormalWarrior'");
		break;
	case UNIT_CODE::NORMAL_HANDTHROWER:
		Path = TEXT("BehaviorTree'/Game/Frameworks/EnemyAI/BT_NormalHandThrower.BT_NormalHandThrower'");
		break;
	case UNIT_CODE::ELITE_KATANA:
		Path = TEXT("BehaviorTree'/Game/Frameworks/EnemyAI/BT_EliteKatana.BT_EliteKatana'");
		break;
	case UNIT_CODE::BOSS_GREATSWORD:
		Path = TEXT("BehaviorTree'/Game/Frameworks/EnemyAI/BT_BossGreatSword.BT_BossGreatSword'");
		break;
	}

	Result = MyGameConfig::LoadObject<UBehaviorTree>(*Path);

	return Result;
}

TObjectPtr<UBlackboardData> GameUnitInitializer::GetBlackboard(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<UBlackboardData> Result = nullptr;
	FString Path = "";

	switch (Unit->GetUnitCode())
	{
	case UNIT_CODE::NORMAL_WARRIOR:
		Path = TEXT("BlackboardData'/Game/Frameworks/EnemyAI/BB_NormalEnemy.BB_NormalEnemy'");
		break;
	case UNIT_CODE::NORMAL_HANDTHROWER:
		Path = TEXT("BlackboardData'/Game/Frameworks/EnemyAI/BB_NormalEnemy.BB_NormalEnemy'");
		break;
	case UNIT_CODE::ELITE_KATANA:
		Path = TEXT("BlackboardData'/Game/Frameworks/EnemyAI/BB_NormalEnemy.BB_NormalEnemy'");
		break;
	case UNIT_CODE::BOSS_GREATSWORD:
		Path = TEXT("BlackboardData'/Game/Frameworks/EnemyAI/BB_NormalEnemy.BB_NormalEnemy'");
		break;
	}

	Result = MyGameConfig::LoadObject<UBlackboardData>(*Path);

	return Result;
}

void GameUnitInitializer::Init_Character(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<USkeletalMesh> TempMesh = MyGameConfig::LoadObject<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ARPG_Warrior/Demo/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	TObjectPtr<UClass> TempABPC = MyGameConfig::LoadObject<UClass>(TEXT("Class'/Game/Characters/Animations/ABP_Character.ABP_Character_C'"));
	TObjectPtr<UAnimMontage> TempMontageHit = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_Character_Hit.MN_Character_Hit'"));
	TObjectPtr<UAnimMontage> TempMontageExecute = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_Character_Execute.MN_Character_Execute'"));

	if (TempMesh != nullptr)
	{
		Unit->GetMesh()->SetSkeletalMeshAsset(TempMesh);
		Unit->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	if (TempABPC != nullptr)
	{
		Unit->GetMesh()->SetAnimInstanceClass(TempABPC);
	}

	if (TempMontageHit != nullptr)
	{
		Unit->SetAnimMontageHit(TempMontageHit);
	}

	if (TempMontageExecute != nullptr)
	{
		Unit->SetAnimMontageExecute(TempMontageExecute);
	}

	Unit->SetEquipmentR(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::RIGHT_HAND, TEXT("TestEquipment1"), UOneHandSwordComponent::StaticClass()));
	Unit->SetEquipmentL(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::LEFT_HAND, TEXT("TestEquipment2"), UShieldComponent::StaticClass()));

	Unit->GetEquipmentR()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);
	Unit->GetEquipmentL()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);

	Unit->GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameUnit"));
	Unit->SetForceWaitState(Unit->GetForceWaitState());
}

void GameUnitInitializer::Init_NormalWarrior(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<USkeletalMesh> TempMesh = MyGameConfig::LoadObject<USkeletalMesh>(TEXT("SkeletalMesh'/Game/EnemyWarriorAnimPack/SkeletalMesh/SK_Mannequin_UE4_WithWeapon.SK_Mannequin_UE4_WithWeapon'"));
	TObjectPtr<UClass> TempABPC = MyGameConfig::LoadObject<UClass>(TEXT("Class'/Game/Characters/Animations/ABP_Enemy_NormalWarrior.ABP_Enemy_NormalWarrior_C'"));
	TObjectPtr<UAnimMontage> TempMontageHit = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_NormalWarrior_Hit.MN_NormalWarrior_Hit'"));
	TObjectPtr<UAnimMontage> TempMontageExecuted = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_NormalWarrior_Executed.MN_NormalWarrior_Executed'"));

	if (TempMesh != nullptr)
	{
		Unit->GetMesh()->SetSkeletalMeshAsset(TempMesh);
		Unit->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -95), FRotator(0, -90, 0));
	}

	if (TempABPC != nullptr)
	{
		Unit->GetMesh()->SetAnimInstanceClass(TempABPC);
	}

	if (TempMontageHit != nullptr)
	{
		Unit->SetAnimMontageHit(TempMontageHit);
	}

	if (TempMontageExecuted != nullptr)
	{
		Unit->SetAnimMontageExecuted(TempMontageExecuted);
	}

	Unit->SetEquipmentR(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::RIGHT_HAND, TEXT("TestEquipment1"), UEmptyOneHandSwordComponent::StaticClass()));
	Unit->GetEquipmentR()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);

	Unit->AIControllerClass = AEnemyAIController::StaticClass();
	Unit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Unit->GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameUnit"));
	Unit->SetForceWaitState(Unit->GetForceWaitState());
}

void GameUnitInitializer::Init_NormalHandThrower(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<USkeletalMesh> TempMesh = MyGameConfig::LoadObject<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ARPG_Warrior/Demo/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	TObjectPtr<UClass> TempABPC = MyGameConfig::LoadObject<UClass>(TEXT("Class'/Game/Characters/Animations/ABP_Enemy_NormalHandThrower.ABP_Enemy_NormalHandThrower_C'"));
	TObjectPtr<UAnimMontage> TempMontageHit = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_NormalHandThrower_Hit.MN_NormalHandThrower_Hit'"));
	TObjectPtr<UAnimMontage> TempMontageExecuted = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_NormalHandThrower_Executed.MN_NormalHandThrower_Executed'"));

	if (TempMesh != nullptr)
	{
		Unit->GetMesh()->SetSkeletalMeshAsset(TempMesh);
		Unit->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -95), FRotator(0, -90, 0));
	}

	if (TempABPC != nullptr)
	{
		Unit->GetMesh()->SetAnimInstanceClass(TempABPC);
	}

	if (TempMontageHit != nullptr)
	{
		Unit->SetAnimMontageHit(TempMontageHit);
	}

	if (TempMontageExecuted != nullptr)
	{
		Unit->SetAnimMontageExecuted(TempMontageExecuted);
	}

	Unit->SetEquipmentR(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::RIGHT_HAND, TEXT("TestEquipment1"), UEmptyHandThrowerComponent::StaticClass()));
	Unit->GetEquipmentR()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);

	Unit->AIControllerClass = AEnemyAIController::StaticClass();
	Unit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Unit->GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameUnit"));
	Unit->SetForceWaitState(Unit->GetForceWaitState());
}

void GameUnitInitializer::Init_EliteKatana(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<USkeletalMesh> TempMesh = MyGameConfig::LoadObject<USkeletalMesh>(TEXT("SkeletalMesh'/Game/HandsomeKatanaAnims/Demo/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	TObjectPtr<UClass> TempABPC = MyGameConfig::LoadObject<UClass>(TEXT("AnimBlueprint'/Game/Characters/Animations/Boss/ABP_EliteKatana.ABP_EliteKatana_C'"));
	TObjectPtr<UAnimMontage> TempMontageHit = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_EliteKatana_Hit.MN_EliteKatana_Hit'"));
	TObjectPtr<UAnimMontage> TempMontageExecuted = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_EliteKatana_Executed.MN_EliteKatana_Executed'"));
	TObjectPtr<UAnimMontage> TempMontageEvade = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_EliteKatana_Evade.MN_EliteKatana_Evade'"));

	if (TempMesh != nullptr)
	{
		Unit->GetMesh()->SetSkeletalMeshAsset(TempMesh);
		Unit->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -95), FRotator(0, -90, 0));
	}

	if (TempABPC != nullptr)
	{
		Unit->GetMesh()->SetAnimInstanceClass(TempABPC);
	}

	if (TempMontageHit != nullptr)
	{
		Unit->SetAnimMontageHit(TempMontageHit);
	}

	if (TempMontageExecuted != nullptr)
	{
		Unit->SetAnimMontageExecuted(TempMontageExecuted);
	}

	if (TempMontageEvade != nullptr)
	{
		Unit->SetAnimMontageEvade(TempMontageEvade);
	}

	Unit->SetEquipmentR(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::RIGHT_HAND, TEXT("TestEquipment1"), UElitesKatanaComponent::StaticClass()));
	Unit->GetEquipmentR()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);

	Unit->AIControllerClass = AEnemyAIController::StaticClass();
	Unit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Unit->GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameUnit"));
	Unit->SetForceWaitState(Unit->GetForceWaitState());
}

void GameUnitInitializer::Init_BossGreatSwrod(TObjectPtr<AGameUnit> Unit)
{
	TObjectPtr<USkeletalMesh> TempMesh = MyGameConfig::LoadObject<USkeletalMesh>(TEXT("SkeletalMesh'/Game/BossyEnemy/SkeletalMesh/SK_Mannequin_UE4_WithWeapon.SK_Mannequin_UE4_WithWeapon'"));
	TObjectPtr<UClass> TempABPC = MyGameConfig::LoadObject<UClass>(TEXT("Class'/Game/Characters/Animations/Boss/ABP_BossGreatSword.ABP_BossGreatSword_C'"));
	TObjectPtr<UAnimMontage> TempMontageHit = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_BossGreatSword_Hit.MN_BossGreatSword_Hit'"));
	TObjectPtr<UAnimMontage> TempMontageExecuted = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/Boss/MN_BossGreatSword_Executed.MN_BossGreatSword_Executed'"));

	if (TempMesh != nullptr)
	{
		Unit->GetMesh()->SetSkeletalMeshAsset(TempMesh);
		Unit->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -95), FRotator(0, -90, 0));
	}

	if (TempABPC != nullptr)
	{
		Unit->GetMesh()->SetAnimInstanceClass(TempABPC);
	}

	if (TempMontageHit != nullptr)
	{
		Unit->SetAnimMontageHit(TempMontageHit);
	}

	if (TempMontageExecuted != nullptr)
	{
		Unit->SetAnimMontageExecuted(TempMontageExecuted);
	}

	Unit->SetEquipmentR(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::RIGHT_HAND, TEXT("TestEquipment1"), UBossyGreatSwordComponent::StaticClass()));
	Unit->GetEquipmentR()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);

	Unit->SetEquipmentL(UEquipmentComponent::CreateEquipment(Unit, Unit->GetMesh(), EQUIP_HAND_TYPE::LEFT_HAND, TEXT("TestEquipment2"), UEmptyHandsComponent::StaticClass()));
	Unit->GetEquipmentL()->Delegate_UseStamina.AddUObject(Unit->GetStatusComponent(), &UCharacterStatusComponent::AddStamina);

	Unit->AIControllerClass = AEnemyAIController::StaticClass();
	Unit->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Unit->GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameUnit"));
	Unit->SetForceWaitState(Unit->GetForceWaitState());
}
