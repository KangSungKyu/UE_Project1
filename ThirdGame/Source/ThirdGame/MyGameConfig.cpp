// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameConfig.h"
#include "UObject/UObjectGlobals.h"
#include "GameUnit.h"


MyGameConfig::MyGameConfig()
{
}

bool MyGameConfig::IsForwardHitDirection(const AGameUnit* AttackUnit, const AGameUnit* HitUnit, float ACrossAngle)
{
	HIT_DIRECTION_TYPE Result = GetHitDirectionType(AttackUnit, HitUnit, ACrossAngle);

	return !(HIT_DIRECTION_TYPE::LEFT <= Result && Result <= HIT_DIRECTION_TYPE::RIGHT);
}

HIT_DIRECTION_TYPE MyGameConfig::GetHitDirectionType(const AGameUnit* AttackUnit, const AGameUnit* HitUnit, float ACrossAngle)
{
	HIT_DIRECTION_TYPE Result = HIT_DIRECTION_TYPE::FORWARD;
	FVector FowardVector = HitUnit->GetActorForwardVector();
	FVector Dir = (AttackUnit->GetActorLocation() - HitUnit->GetActorLocation()).GetSafeNormal();
	FVector Cross = FVector::CrossProduct(FowardVector, Dir);
	float Dot = FVector::DotProduct(FowardVector, Dir);
	float CAngle = FowardVector.CosineAngle2D(Dir);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(CAngle));
	float Adjust = (90 - ACrossAngle) * 0.5f;
	bool bRight = Cross.Z >= 0;

	if (0 <= Angle && Angle < Adjust)
	{
		Result = HIT_DIRECTION_TYPE::FORWARD;
	}
	else if (Adjust <= Angle && Angle <= 90 - Adjust)
	{
		Result = bRight ? HIT_DIRECTION_TYPE::FORWARD_RIGHT : HIT_DIRECTION_TYPE::FORWARD_LEFT;
	}
	else if (90 - Adjust < Angle && Angle < 90 + Adjust)
	{
		Result = bRight ? HIT_DIRECTION_TYPE::RIGHT : HIT_DIRECTION_TYPE::LEFT;
	}
	else if (90 + Adjust <= Angle && Angle <= 180 - Adjust)
	{
		Result = bRight ? HIT_DIRECTION_TYPE::BACKWARD_RIGHT : HIT_DIRECTION_TYPE::BACKWARD_LEFT;
	}
	else if (180 - Adjust < Angle && Angle <= 180)
	{
		Result = HIT_DIRECTION_TYPE::BACKWARD;
	}

	return Result;
}

MyGameConfig::~MyGameConfig()
{
}

EHardwareDevicePrimaryType MyGameConfig::GetMostRecentlyHardwareDeviceType(const ACharacter* Character)
{
	UInputDeviceSubsystem* InputDeviceSubsystem = Character->GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();

	return InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(Character->GetPlatformUserId()).PrimaryDeviceType;
}
