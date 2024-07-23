## 언리얼 포트폴리오
사용 엔진 : 언리얼 5.3.2 <br>
편집기 : Visual Studio 2022<br>
ARPG의 기본적인 기능들을 구현해봤습니다.

## 캐릭터 구조 (ACharacter - AController)
![유닛 클래스](https://github.com/user-attachments/assets/6b0cd32c-29b1-49a7-8697-6ae0150d8ee5)
![유닛 스탯](https://github.com/user-attachments/assets/419b5f9b-cc6d-404a-a0de-472443d4249f)<br>

### AGameUnit
UEquipmentComponent 관리<br>
UCharacterStatuscComponent를 통해 캐릭터 자원(체력, 데미지, 스태미너, ...) 관리<br>
UPlayerUI에 체력, 스태미너가 변할때 event처리를 위한 함수포인터 전달<br>

```cpp
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
```

### AMyCharacter
UInputMappingContext, UInputAction으로 유저가 입력한 값에 따라 행동합니다. (이동, 달리기, 락온, 일반 공격, 차지 공격, 가드, 패링)<br>
IGenericTeamAgentInterface을 상속 받아 AEnemy의 APerception에 감지될 수 있습니다.<br>
적의 공격을 정확하게 피하거나 막기위해 시점 고정 기능을 사용할 수 있습니다<br>
```cpp
void AMyCharacter::Tick(float TickDelta)
{
	Super::Tick(TickDelta);

	if (!IsAvailableBehavior())
	{
		return;
	}

	if (bLockOn && TargetCharacter)
	{
		FVector CamStartLocation = Camera->GetComponentLocation();
		FVector TargetLocation = TargetCharacter->GetLockOnSocketLocation();
		FRotator CamLookAt = UKismetMathLibrary::FindLookAtRotation(CamStartLocation, TargetLocation);

		FRotator CamRot = FMath::RInterpTo(Camera->GetComponentRotation(), CamLookAt, TickDelta, 1.0f);
		FRotator CharacterRot = FRotator(0, CamLookAt.Yaw, 0);

		SpringArm->SetWorldRotation(FRotator(-20, CamLookAt.Yaw, 0));
		Controller->SetControlRotation(FRotator(0, MovementState == MOVEMENT_STATE::WALK ? CamRot.Yaw : CamLookAt.Yaw, 0));

		if (MovementState == MOVEMENT_STATE::WALK && EvadeState == 0)
		{
			SetActorRotation(CharacterRot);
		}

		if (IsOcclusionTo(CamStartLocation, TargetCharacter))
		{
			OcclusionTime += TickDelta;

			if (OcclusionTime >= 2.0f)
			{
				SetTargetCharacter(nullptr);
				OcclusionTime = 0.0f;
			}
		}
		else
		{
			OcclusionTime = 0.0f;
		}
	}
	else
	{
		OcclusionTime = 0.0f;
	}
}
```
락온 여부와 이동 상태에 따라 캐릭터의 이동 방향 및 회피 방향이 달라집니다.<br>
| 락온          | 이동상태       | 방향    |
| ------------- | -------------  | ------- |
| x             | 걷기, 달리기, 구르기 |  8방향   |
| o             | 걷기           | 타켓 방향 |
| o             | 달리기, 구르기  | 8방향    |
<br>
락온 대상과 카메라가 일정시간 가려지면 자동으로 풀립니다. <br>

### UEquipmentComponent
![장비 클래스](https://github.com/user-attachments/assets/7bd39806-a5a5-4894-84b8-7c7b3bc386cc)<br>
캐릭터마다 사용할 장비의 Mesh와 BoxComponent를 관리합니다.<br>
장비마다 각각의 Montage를 불러와 사용합니다.<br>
```cpp
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
```
Montage마다 UAnimNotifyState를 상속받은 UCollisionStateNotify를 세팅 및 CheckSweepCollision를 호출하여 데미지 및 가드처리를 관리합니다.<br>

### AEnemy
체력 ui(UWidgetComponent)에 event처리를 위한 함수포인터 전달<br>
UAIPerceptionComponent, UAISenseConfig_Sight로 시야 감지 및 BehaviourTree에 정보 전달<br>
![일반 몹 ai](https://github.com/user-attachments/assets/c38094a5-aae7-45fc-944a-0081d1161416)<br>
일반 적은 위와 같은 BT로 동작합니다.<br><br>
![보스 ai 1](https://github.com/user-attachments/assets/4e588550-d3d0-4230-926a-0470a09ee415)<br>
보스는 적과의 거리에 따라 행동이 달라집니다.<br><br>

