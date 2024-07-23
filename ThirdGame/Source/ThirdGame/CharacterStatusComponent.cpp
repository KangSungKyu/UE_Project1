// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusComponent.h"
#include "GameUnit.h"
#include "EquipmentComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCharacterStatusComponent::UCharacterStatusComponent()
{
	PoiseRecoverTime = 2.5f;
	PrimaryComponentTick.bCanEverTick = true;
	Owner = nullptr;

	Delegate_ChangeHP.Clear();
	Delegate_ChangeMaxHP.Clear();
	Delegate_ChangeStamina.Clear();
	Delegate_ChangeMaxStamina.Clear();
	Delegate_RecoverPoise.Clear();
	Delegate_Stagger.Clear();
	// ...
}

void UCharacterStatusComponent::SetStatus(UNIT_SIZE_TYPE SizeType, const FCharacterStat& Stat)
{
	Status = FCharacterStatus::Build(SizeType, Stat);

	PrimaryComponentTick.bCanEverTick = Status.MaxHp > 0 && Status.MaxStamina > 0;

	Delegate_ChangeMaxHP.Broadcast(Status.MaxHp);
	Delegate_ChangeHP.Broadcast(Status.Hp, Status.MaxHp);
	Delegate_ChangeMaxStamina.Broadcast(Status.MaxStamina);
	Delegate_ChangeStamina.Broadcast(Status.Stamina, Status.MaxStamina);
}

bool UCharacterStatusComponent::Damaged(float Damage, TObjectPtr<AGameUnit> Causer)
{
	bool bDamage = true;
	bool bHitReaction = false;
	float HitPoint = Damage * 2.5f;
	float GuardHitPoint = HitPoint * 0.625f;

	if (Owner->IsOnShield() && MyGameConfig::IsForwardHitDirection(Causer, Owner))
	{
		bDamage = false;
		AddStamina(-GuardHitPoint);

		if (Owner->GetShieldState() != 1)
		{
			CalcPoisePoint(HitPoint);
			Owner->SetHitReaction(true);

			if (Status.Stamina <= 0)
			{
				SetPoisePoint(0);
			}
		}
	}
	else
	{
		Status.Hp -= Damage;
		Status.Hp = UKismetMathLibrary::FClamp(Status.Hp, 0, Status.MaxHp);

		Owner->SetHitReaction(Status.HitRecovery <= HitPoint);
		Delegate_ChangeHP.Broadcast(Status.Hp, Status.MaxHp);

		if (Status.PoisePoint > 0)
		{
			CalcPoisePoint(HitPoint);
		}
		else
		{
			SetPoisePoint(Status.MaxPoisePoint);
		}
	}
	
	return bDamage;
}

bool UCharacterStatusComponent::ExecutionDamaged(float Damage, TObjectPtr<AGameUnit> Causer)
{
	bool bDamage = true;

	Status.Hp -= Damage;
	Status.Hp = UKismetMathLibrary::FClamp(Status.Hp, 0, Status.MaxHp);

	Delegate_ChangeHP.Broadcast(Status.Hp, Status.MaxHp);

	SetPoisePoint(Status.MaxPoisePoint);

	return bDamage;
}

bool UCharacterStatusComponent::IsEnoughStamina(float Stamina)
{
	return Status.Stamina > 0 && Status.Stamina >= Stamina;
}

void UCharacterStatusComponent::ResetPoisePoint()
{
	if (Status.bChargePoisePoint)
	{
		Status.bChargePoisePoint = false;
		Status.PoisePoint = Status.InitialPoisePoint;
	}
}

void UCharacterStatusComponent::ChargePoisePoint()
{
	if (!Status.bChargePoisePoint)
	{
		Status.bChargePoisePoint = true;
		Status.PoisePoint = Status.MaxPoisePoint;
	}
}

void UCharacterStatusComponent::SetPoisePoint(float PoisePoint)
{
	Status.PoisePoint = PoisePoint;

	if (Status.PoisePoint >= Status.MaxPoisePoint)
	{
		Delegate_RecoverPoise.Broadcast();
		Owner->GetWorldTimerManager().ClearTimer(PoiseTimerHandle);
	}
	else if (Status.PoisePoint <= 0)
	{
		Delegate_Stagger.Broadcast();
	}
}


void UCharacterStatusComponent::AddStamina(float Stamina)
{
	bool bValid = Stamina > 0 ? true : IsEnoughStamina(Stamina);

	if (bValid)
	{
		Status.Stamina = UKismetMathLibrary::FClamp(Status.Stamina + Stamina, 0, Status.MaxStamina);

		Delegate_ChangeStamina.Broadcast(Status.Stamina, Status.MaxStamina);

		//UE_LOG(LogTemp, Log, TEXT("stamina %.2f"), Status.Stamina);
	}
}

// Called when the game starts
void UCharacterStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AGameUnit>(GetOwner());
	// ...
}

void UCharacterStatusComponent::SetPoiseTimer()
{
	if (Owner->GetWorldTimerManager().IsTimerActive(PoiseTimerHandle))
	{
		Owner->GetWorldTimerManager().ClearTimer(PoiseTimerHandle);
	}

	Owner->GetWorldTimerManager().SetTimer(PoiseTimerHandle, this, &UCharacterStatusComponent::PoiseTimer, 0.1f, true, PoiseRecoverTime);
}

void UCharacterStatusComponent::PoiseTimer()
{
	float CurrPoise = Status.PoisePoint;

	CurrPoise += (Status.MaxPoisePoint * 0.1f * 0.5f);
	CurrPoise = UKismetMathLibrary::FClamp(CurrPoise, 0, Status.MaxPoisePoint);

	SetPoisePoint(CurrPoise);
}

void UCharacterStatusComponent::CalcPoisePoint(float HitPoint)
{
	Status.PoisePoint -= (HitPoint * 0.25f);
	Status.PoisePoint = UKismetMathLibrary::FClamp(Status.PoisePoint, 0, Status.MaxPoisePoint);

	SetPoiseTimer();
}


// Called every frame
void UCharacterStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Owner->IsRecoveryStamina() && Status.Stamina < Status.MaxStamina)
	{
		float StaminaFactor = Owner->IsHitReaction() ? 0.725f : 1.0f;
		float DeltaStamina = Status.StaminaPerSec * StaminaFactor * DeltaTime;
		
		AddStamina(DeltaStamina);

		//UE_LOG(LogTemp, Log, TEXT("stamina %.2f"), Status.Stamina);
	}
	else if(Owner->GetMovementState() == MOVEMENT_STATE::RUN)
	{
		float DeltaStamina = 15 * DeltaTime;

		AddStamina(-DeltaStamina);

		//UE_LOG(LogTemp, Log, TEXT("stamina %.2f"), Status.Stamina);
	}
}

