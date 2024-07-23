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

### UEquipmentComponent
![장비 클래스](https://github.com/user-attachments/assets/7bd39806-a5a5-4894-84b8-7c7b3bc386cc)<br>
캐릭터마다 사용할 장비의 Mesh와 BoxComponent를 관리합니다.<br>
장비마다 각각의 Montage를 불러와 사용합니다.<br>

## AEnemy
체력 ui(UWidgetComponent)에 event처리를 위한 함수포인터 전달<br>
UAIPerceptionComponent, UAISenseConfig_Sight로 시야 감지 및 BehaviourTree에 정보 전달<br>
