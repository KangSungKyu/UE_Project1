// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "GameFramework/Character.h"
#include "GameUnit.h"
#include "CharacterStatusComponent.h"
#include "Kismet/GameplayStatics.h"


 TObjectPtr<UEquipmentComponent> UEquipmentComponent::CreateEquipment(TObjectPtr<AGameUnit> Owner, TObjectPtr<UMeshComponent> OwnerMesh, EQUIP_HAND_TYPE HandType, const FName& ItemName, TSubclassOf<UEquipmentComponent> SubClass)
 {
	 TObjectPtr<UEquipmentComponent> Equipment = nullptr;

	 if (Owner != nullptr)
	 {
		 Equipment = ::NewObject<UEquipmentComponent>(Owner, *SubClass, ItemName);

		 Equipment->OwnerCharacter = Owner;
		 Equipment->EquipHandType = HandType;
		 Equipment->EquipmentName = ItemName;

		 Equipment->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *Equipment->GetEquipSocketName());
		 Equipment->CreateMeshAndBox();
		 Equipment->InitEquipment(OwnerMesh);
	 }

	 return Equipment;
 }

 bool UEquipmentComponent::CheckShield()
 {
	 if (EquipmentType == EQUIPMENT_TYPE::SHIELD)
	 {
		 return State != StateEnd;
	 }

	 return false;
 }

 bool UEquipmentComponent::IsAvailableBehavior(TObjectPtr<UCharacterStatusComponent> StatusComponent, bool bNormal)
 {
	 bool bSuccess = true;
	 
	 if (bNormal)
	 {
		 bSuccess = StatusComponent->IsEnoughStamina(NormalCost);
	 }
	 else
	 {
	 	 bSuccess = StatusComponent->IsEnoughStamina(ChargeCost);
	 }

	 return bSuccess;
 }

 void UEquipmentComponent::SetBoxCollision(bool bValue)
 {
	 if (BoxComponent != nullptr)
	 {
		 ECollisionEnabled::Type CollType = ECollisionEnabled::Type::NoCollision;

		 if (bValue)
		 {
			 CollType = ECollisionEnabled::Type::QueryOnly;
		 }

		 BoxComponent->SetCollisionEnabled(CollType);
	 }
 }

 bool UEquipmentComponent::CheckSweepCollision(FHitResult& HitResult)
 {
	 bool bResult = false;

	 if (BoxComponent != nullptr && BoxComponent->GetCollisionEnabled() != ECollisionEnabled::Type::NoCollision)
	 {
		 FVector BoxPos = EquipmentMesh->GetComponentLocation();
		 FQuat BoxQuat = BoxComponent->GetComponentRotation().Quaternion();
		 FCollisionQueryParams Params(NAME_None, false, OwnerCharacter);

		 bResult = GetWorld()->SweepSingleByChannel
		 (
			 HitResult,
			 BoxPos,
			 BoxPos,
			 BoxQuat,
			 ECollisionChannel::ECC_GameTraceChannel2,
			 FCollisionShape::MakeBox(BoxComponent->GetScaledBoxExtent() * 0.5f),
			 Params
		 );

		 //DrawDebugBox(GetWorld(), BoxPos, BoxComponent->GetScaledBoxExtent(), BoxQuat, bResult ? FColor::Green : FColor::Red, false, 0.4f);

		 if (bResult)
		 {
			 TObjectPtr<UPrimitiveComponent> HitComponent = HitResult.GetComponent();
			 TObjectPtr<UEquipmentComponent> GuardableEquipment = Cast<UEquipmentComponent>(HitComponent->GetAttachParent());
			 TObjectPtr<AGameUnit> Unit = nullptr;

			 if (GuardableEquipment != nullptr)
			 {
				 Unit = GuardableEquipment->OwnerCharacter;

				 if (GuardableEquipment->GetState() == 1)
				 {
					 OwnerCharacter->GetStatusComponent()->SetPoisePoint(0);
					 OwnerCharacter->GetStatusComponent()->SetPoiseTimer();

					 return bResult;
				 }
			 }
			 else if (HitResult.GetActor()->IsValidLowLevel())
			 {
				 Unit = Cast<AGameUnit>(HitResult.GetActor());
			 }

			 if (Unit != nullptr && Unit->ActorHasTag(OwnerCharacter->GetHostileUnitTag()) && !Unit->IsHitReaction())
			 {
				 UGameplayStatics::ApplyDamage(Unit, OwnerCharacter->GetStatus().Attack, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
			 }
		 }

	 } 

	 return bResult;
 }

 // Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	EquipmentType = EQUIPMENT_TYPE::NONE;
	EquipHandType = EQUIP_HAND_TYPE::RIGHT_HAND;
	State = 0;
	ChargeState = 0;
	StateEnd = 0;
	ChargeCycleCount = 0;
	ChargeStateEnd = 0;
	NormalCost = 0;
	ChargeCost = 0;
	EquipmentName = "";
	EquipmentMesh = nullptr;
	BoxComponent = nullptr;
	NormalMontage = nullptr;
	ChargeMontage = nullptr;
	OwnerCharacter = nullptr;

	Delegate_UseStamina.Clear();
	// ...
}

void UEquipmentComponent::CreateMeshAndBox()
{
	EquipmentMesh = ::NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("StaticMesh"));
	BoxComponent = ::NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), TEXT("MeshBox"));

	EquipmentMesh->RegisterComponent();
	BoxComponent->RegisterComponent();
	EquipmentMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	SetBoxCollision(false);
}

FString UEquipmentComponent::GetEquipSocketName()
{
	FString SocketName = "";
	FString TypeName = "";
	FString HandName = "";

	switch (EquipmentType)
	{
	case EQUIPMENT_TYPE::WEAPON:
		TypeName = "Weapon";
		break;
	case EQUIPMENT_TYPE::SHIELD:
		TypeName = "Shield";
		break;
	default:
		//nothing
		break;
	}

	switch (EquipHandType)
	{
	case EQUIP_HAND_TYPE::RIGHT_HAND:
		HandName = "R";
		break;
	case EQUIP_HAND_TYPE::LEFT_HAND:
		HandName = "L";
		break;
	default:
		//nothing
		break;
	}

	SocketName = FString::Printf(TEXT("%s_%s"), *TypeName, *HandName);

	return SocketName;
}

bool UEquipmentComponent::IsAvailable()
{
	TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	bool bAvailablePlay = true;

	if (AnimInstance)
	{
		if (State == 0)
		{
			bAvailablePlay = !AnimInstance->Montage_IsPlaying(NormalMontage);
		}
	}

	return bAvailablePlay;
}

bool UEquipmentComponent::IsAvailableCharge()
{
	TObjectPtr<UAnimInstance> AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	bool bAvailablePlay = true;

	if (AnimInstance)
	{
		if (ChargeState % CHARGE_COMBO_UNIT == CHARGE_COMBO_STEP_START)
		{
			bAvailablePlay = !AnimInstance->Montage_IsPlaying(ChargeMontage);
		}
	}

	return bAvailablePlay;
}

void UEquipmentComponent::PlayStart()
{
}

void UEquipmentComponent::PlayTrigger()
{
}

void UEquipmentComponent::PlayCompleted()
{
}

void UEquipmentComponent::PlayChargeStart()
{
}

void UEquipmentComponent::PlayChargeTrigger()
{
}

void UEquipmentComponent::PlayChargeCanceled()
{
}

void UEquipmentComponent::PlayChargeCompleted()
{
}
