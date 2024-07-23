// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "MyGameConfig.generated.h"

class UInputDeviceSubsystem;
class AGameUnit;

/**
 * 
 */

const static uint8 ATTACK_COMBO_UNIT = 1; //attack
const static uint8 CHARGE_COMBO_UNIT = 3; //start-charge-attack
const static uint8 CHARGE_COMBO_STEP_START = 0;
const static uint8 CHARGE_COMBO_STEP_CHARGE = 1;
const static uint8 CHARGE_COMBO_STEP_ATTACK = 2;
const static float MAX_LOCKON_DISTANCE = 2500.0f;

UENUM(BlueprintType)
enum class UNIT_TYPE : uint8
{
	NPC,
	ALLY,
	ENEMY,

	UNIT_TYPE_END
};

UENUM(BlueprintType)
enum class UNIT_SIZE_TYPE : uint8
{
	SMALL,
	NORMAL,
	ELITE,
	BOSS,

	UNIT_SIZE_TYPE_END
};

UENUM(BlueprintType)
enum class UNIT_CODE
{
	NONE,
	CHARACTER,
	NORMAL_WARRIOR,
	NORMAL_HANDTHROWER,
	ELITE_KATANA,
	BOSS_GREATSWORD,

	UNIT_CODE_END
};

UENUM(BlueprintType)
enum class MOVEMENT_STATE : uint8
{
	WALK,
	RUN,

	MOVEMENT_STATE_END
};

UENUM(BlueprintType)
enum class ATTACK_STATE : uint8
{
	ATTACK_1,
	ATTACK_2,
	ATTACK_3,
	ATTACK_4,

	ATTACK_STATE_END
};

UENUM(BlueprintType)
enum class CHARGE_ATTACK_STATE : uint8
{
	START_1,
	CHARGE_1,
	CHARGE_ATTACK_1,
	START_2,
	CHARGE_2,
	CHARGE_ATTACK_2,

	CHARGE_ATTACK_STATE_END
};

UENUM(BlueprintType)
enum class CHARACTER_STATE : uint8
{
	DEATH,
	ALIVE,
};

UENUM(BlueprintType)
enum class EQUIPMENT_TYPE : uint8
{
	NONE = 0,
	WEAPON,
	SHIELD,
};

UENUM(BlueprintType)
enum class EQUIP_HAND_TYPE : uint8
{
	RIGHT_HAND,
	LEFT_HAND,
};

UENUM(BlueprintType)
enum class HIT_DIRECTION_TYPE : uint8
{
	FORWARD,
	FORWARD_LEFT,
	LEFT,
	BACKWARD_LEFT,
	BACKWARD,
	BACKWARD_RIGHT,
	RIGHT,
	FORWARD_RIGHT,

	HIT_DIRECTION_TYPE_END
};

UENUM(BlueprintType)
enum class PATROL_TYPE : uint8
{
	NONE,
	RANDOM,
	REGION,
	TWO_POINT,

	PATROL_TYPE_END
};

DECLARE_MULTICAST_DELEGATE_OneParam(FStatusDelegate_UseStamina, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FStatusDelegate_ChangeHP, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FStatusDelegate_ChangeMaxHP, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FStatusDelegate_ChangeStamina, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FStatusDelegate_ChangeMaxStamina, float);
DECLARE_MULTICAST_DELEGATE(FStatusDelegate_RecoverPoise);
DECLARE_MULTICAST_DELEGATE(FStatusDelegate_Stagger);
DECLARE_MULTICAST_DELEGATE(FScriptDelegate_TrapTrigger);


USTRUCT(BlueprintType)
struct FCharacterStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Vit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Endurance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Str;
};

USTRUCT(BlueprintType)
struct FCharacterStatus
{
	GENERATED_BODY()

public:
	static FCharacterStatus Build(UNIT_SIZE_TYPE Type, const FCharacterStat& Stat)
	{
		FCharacterStatus Status;

		Status.bChargePoisePoint = false;

		Status.Attack = Stat.Str * 5 + Stat.Endurance * 0.5f;
		Status.MaxHp = Stat.Vit * 20;
		Status.MaxStamina = Stat.Endurance * 15;
		Status.StaminaPerSec = 20;

		switch (Type)
		{
		case UNIT_SIZE_TYPE::SMALL:
			Status.HitRecovery = 50;
			Status.MaxPoisePoint = 60;
			Status.InitialPoisePoint = 0;
			break;
		case UNIT_SIZE_TYPE::NORMAL:
			Status.HitRecovery = 100;
			Status.MaxPoisePoint = 100;
			Status.InitialPoisePoint = 0;
			break;
		case UNIT_SIZE_TYPE::ELITE:
			Status.HitRecovery = 200;
			Status.MaxPoisePoint = 150;
			Status.InitialPoisePoint = 150;
			break;
		case UNIT_SIZE_TYPE::BOSS:
			Status.HitRecovery = 300;
			Status.MaxPoisePoint = 500;
			Status.InitialPoisePoint = 500;
			break;
		}

		Status.Hp = Status.MaxHp;
		Status.Stamina = Status.MaxStamina;
		Status.PoisePoint = Status.InitialPoisePoint;

		return Status;
	}

public:
	FCharacterStatus() 
	{
		bChargePoisePoint = false;
		Attack = 0;
		Hp = 0;
		MaxHp = 0;
		Stamina = 0;
		MaxStamina = 0;
		StaminaPerSec = 0;
		HitRecovery = 0;
		PoisePoint = 0;
		MaxPoisePoint = 0;
		InitialPoisePoint = 0;
	}

public:
	bool bChargePoisePoint;

	UPROPERTY(BlueprintReadWrite)
	float Attack;

	UPROPERTY(BlueprintReadWrite)
	float Hp;

	UPROPERTY(BlueprintReadWrite)
	float MaxHp;

	UPROPERTY(BlueprintReadWrite)
	float Stamina;

	UPROPERTY(BlueprintReadWrite)
	float MaxStamina;

	UPROPERTY(BlueprintReadWrite)
	float StaminaPerSec;

	UPROPERTY(BlueprintReadWrite)
	float HitRecovery;

	UPROPERTY(BlueprintReadWrite)
	float PoisePoint;

	UPROPERTY(BlueprintReadWrite)
	float MaxPoisePoint;

	UPROPERTY(BlueprintReadWrite)
	float InitialPoisePoint;
};

class THIRDGAME_API MyGameConfig
{
public:
	template<class T>
	static TObjectPtr<T> LoadObject(const TCHAR* Path)
	{
		return ::LoadObject<T>(nullptr, Path, nullptr, LOAD_None, nullptr);
	}

	static EHardwareDevicePrimaryType GetMostRecentlyHardwareDeviceType(const ACharacter* Character);
	static HIT_DIRECTION_TYPE GetHitDirectionType(const AGameUnit* AttackUnit, const AGameUnit* HitUnit, float ACrossAngle = 60.0f);
	static bool IsForwardHitDirection(const AGameUnit* AttackUnit, const AGameUnit* HitUnit, float ACrossAngle = 60.0f);
public:
	MyGameConfig();
	~MyGameConfig();
};
