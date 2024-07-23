// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUnit.h"
#include "EquipmentComponent.h"
#include "CharacterStatusComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGameUnit::AGameUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

FCharacterStatus AGameUnit::GetStatus()
{
	return StatusComponent->GetStatus();
}

UCharacterStatusComponent* AGameUnit::GetStatusComponent()
{
	return StatusComponent;
}

uint8 AGameUnit::GetShieldState()
{
	if (EquipmentL != nullptr && EquipmentL->CheckShield())
	{
		return EquipmentL->GetState();
	}
	else if (EquipmentR != nullptr && EquipmentR->CheckShield())
	{
		return EquipmentR->GetState();
	}

	return 2;
}

bool AGameUnit::IsPlayAttackAnyEquipment()
{
	if (EquipmentR != nullptr && (EquipmentR->IsPlay() || EquipmentR->IsPlayCharge()))
	{
		return true;
	}
	else if (EquipmentL != nullptr && (EquipmentL->IsPlay() || EquipmentL->IsPlayCharge()))
	{
		return true;
	}

	return false;
}

void AGameUnit::ExecutionStart()
{
	//nothing
}

void AGameUnit::ExecutionEnd()
{
	bExecutionMotion = false;
	SetHitReaction(false);
}

void AGameUnit::SimpleNormalAttackFromEquipmentR()
{
	if (EquipmentR != nullptr)
	{
		EquipmentR->PlayStart();
	}
}

void AGameUnit::SimpleRunAttackFromEquipmentR()
{
	if (EquipmentR != nullptr)
	{
		MovementState = MOVEMENT_STATE::RUN;
		EquipmentR->PlayStart();
		MovementState = MOVEMENT_STATE::WALK;
	}
}

FVector AGameUnit::GetLockOnSocketLocation()
{
	FVector Loc = GetActorLocation();
	FName SocketName = TEXT("LockOnSocket");

	if (GetMesh() != nullptr && GetMesh()->DoesSocketExist(SocketName))
	{
		Loc = GetMesh()->GetSocketLocation(SocketName);
	}

	return Loc;
}

bool AGameUnit::IsRecoveryStamina()
{
	bool Result = true;
	bool IsRun = MovementState == MOVEMENT_STATE::RUN;
	bool IsEquipmentAnim = false;
	bool bEquipmentR = false;
	bool bEquipmentL = false;
	
	if (GetEquipmentR() != nullptr)
	{
		bEquipmentR = GetEquipmentR()->IsPlay() || GetEquipmentR()->IsPlayCharge();
	}

	if (GetEquipmentL() != nullptr)
	{
		bEquipmentL = GetEquipmentL()->IsPlay() || GetEquipmentL()->IsPlayCharge();
	}

	IsEquipmentAnim = bEquipmentR || bEquipmentL;
	Result = !IsRun && !IsEquipmentAnim;

	return Result;
}

void AGameUnit::PlayExecute(const FName& SectionName, AGameUnit* Target)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontageExecute != nullptr)
	{
		AnimInstance->Montage_Play(AnimMontageExecute, 1.0f);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontageExecute);

		bExecutionMotion = true;

		Target->PlayExecuted(SectionName);

		UGameplayStatics::ApplyDamage(Target, GetStatus().Attack, GetController(), this, UDamageType::StaticClass());
	}
}

void AGameUnit::PlayExecuted(const FName& SectionName)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontageExecuted != nullptr)
	{
		AnimInstance->Montage_Play(AnimMontageExecuted, 1.0f);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontageExecuted);

		bExecutionMotion = true;

		ExecutionStart();
	}
}

bool AGameUnit::IsOcclusionTo(const FVector& StartLocation, const TObjectPtr<AGameUnit>& Unit)
{
	bool bResult = false;

	if (Unit != nullptr)
	{
		FHitResult HitResult;
		FCollisionQueryParams QParams(NAME_None, false, this);
		FCollisionResponseParams RParams(ECollisionResponse::ECR_Block);
		FVector EndLocation = Unit->GetActorLocation();

		bool bHit = GetWorld()->LineTraceSingleByChannel
		(
			HitResult,
			StartLocation,
			EndLocation,
			ECC_WorldStatic,
			QParams,
			RParams
		);

		if (bHit && HitResult.GetActor() != nullptr && HitResult.GetActor()->IsValidLowLevel())
		{
			if (Cast<AGameUnit>(HitResult.GetActor()) == nullptr)
			{
				bResult = true;
			}
		}

		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, bResult ? FColor::Magenta : FColor::Cyan, false, 0.1f, 0, 5);
	}

	return bResult;
}

float AGameUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	TObjectPtr<AGameUnit> Causer = Cast<AGameUnit>(DamageCauser);

	Damaged(FinalDamage, Causer);

	return FinalDamage;
}
