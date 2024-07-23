// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "CharacterStatusComponent.h"
#include "GameUnitInitializer.h"
#include "TestActor.h"
#include "Enemy.h"
#include "EquipmentComponent.h"
#include "OneHandSwordComponent.h"
#include "ShieldComponent.h"
#include "InteractObject.h"
#include "PlayerUI.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> TempIMC(TEXT("InputMappingContext'/Game/Input/InputMappingContext.InputMappingContext'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> TempTestActor(TEXT("Blueprint'/Game/Frameworks/BP_TestActor.BP_TestActor'"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	MoveAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_Move.IA_Move'"));
	LookAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_Look.IA_Look'"));
	LockOnAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_LockOn.IA_LockOn'"));
	OrientCameraAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_OrientCamera.IA_OrientCamera'"));
	RunAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_Run.IA_Run'"));
	WeaponR1Action = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_WeaponR1.IA_WeaponR1'"));
	WeaponR2Action = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_WeaponR2.IA_WeaponR2'"));
	WeaponL1Action = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_WeaponL1.IA_WeaponL1'"));
	WeaponL2Action = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_WeaponL2.IA_WeaponL2'"));
	WeaponAbilityAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_WeaponAbility.IA_WeaponAbility'"));
	EvadeAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_Evade.IA_Evade'"));
	DodgeAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_Dodge.IA_Dodge'"));
	InteractAction = MyGameConfig::LoadObject<UInputAction>(TEXT("InputAction'/Game/Input/IA_Interact.IA_Interact'"));

	AnimMontageEvade = MyGameConfig::LoadObject<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Animations/MN_Character_Evade.MN_Character_Evade'"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (TempIMC.Succeeded())
	{
		DefaultMappingContext = TempIMC.Object;
	}

	UnitType = UNIT_TYPE::ALLY;
	SizeType = UNIT_SIZE_TYPE::NORMAL;
	UnitCode = UNIT_CODE::CHARACTER;

	bLockOn = false;
	bWeaponAbility = false;
	bOnShield = false;
	bHitReaction = false;
	bForceWaitState = false;
	MaxWalkSpeed = 400;
	MaxRunSpeed = 700;
	MaxAcceleration = 400;
	MaxGuardWalkSpeed = 75;
	CurrentSavedSpeed = MaxWalkSpeed;
	RunCost = 5;
	EvadeCost = 25;
	MovementState = MOVEMENT_STATE::WALK;
	CharacterState = CHARACTER_STATE::ALIVE;
	UnitTag = TEXT("Game.Unit.Ally");
	TeamId = 1;

	OcclusionTime = 0.0f;

	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	
	//GetCapsuleComponent()->bHiddenInGame = false; //test
	
	SpringArm->bDoCollisionTest = true;
	SpringArm->TargetArmLength = 700;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->CameraLagSpeed = 15;
	SpringArm->CameraRotationLagSpeed = 15;
	SpringArm->SetWorldRotation(FRotator(-10, 0, 0));

	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	
	Camera->FieldOfView = 90;
	Camera->bUsePawnControlRotation = false;
	//Camera->bDrawFrustumAllowed = true; //test

	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->MaxStepHeight = 60;
	GetCharacterMovement()->SetWalkableFloorAngle(30);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000;
	GetCharacterMovement()->CrouchedHalfHeight = 40;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	CrouchedEyeHeight = 90;

	TargetCharacterClass = AEnemy::StaticClass();
	StatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>(TEXT("CharacterStatus"));

	Stat.Vit = 10;
	Stat.Str = 10;
	Stat.Endurance = 15;

	//UE_LOG(LogTemp, Log, TEXT("character spawn 1, %.2f, %.2f, %.2f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

	AIStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIStimuliSource"));
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		GameUnitInitializer::Init(this);

		StatusComponent->Delegate_Stagger.AddUObject(this, &AMyCharacter::OnStagger);
		StatusComponent->SetStatus(SizeType, Stat);
		StatusComponent->ChargePoisePoint();
	}

	//UE_LOG(LogTemp, Log, TEXT("character spawn 2, %.2f, %.2f, %.2f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
}

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

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &AMyCharacter::LockOnMode);

		EnhancedInputComponent->BindAction(OrientCameraAction, ETriggerEvent::Started, this, &AMyCharacter::OrientCamera);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AMyCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyCharacter::Run);

		EnhancedInputComponent->BindAction(WeaponR1Action, ETriggerEvent::Started, this, &AMyCharacter::WeaponR1_Start);
		EnhancedInputComponent->BindAction(WeaponR1Action, ETriggerEvent::Triggered, this, &AMyCharacter::WeaponR1_Trigger);
		EnhancedInputComponent->BindAction(WeaponR1Action, ETriggerEvent::Completed, this, &AMyCharacter::WeaponR1_Complete);

		EnhancedInputComponent->BindAction(WeaponR2Action, ETriggerEvent::Started, this, &AMyCharacter::WeaponR2_Start);
		EnhancedInputComponent->BindAction(WeaponR2Action, ETriggerEvent::Triggered, this, &AMyCharacter::WeaponR2_Trigger);
		EnhancedInputComponent->BindAction(WeaponR2Action, ETriggerEvent::Canceled, this, &AMyCharacter::WeaponR2_Canceled);
		EnhancedInputComponent->BindAction(WeaponR2Action, ETriggerEvent::Completed, this, &AMyCharacter::WeaponR2_Complete);

		EnhancedInputComponent->BindAction(WeaponL1Action, ETriggerEvent::Started, this, &AMyCharacter::WeaponL1_Start);
		EnhancedInputComponent->BindAction(WeaponL1Action, ETriggerEvent::Triggered, this, &AMyCharacter::WeaponL1_Trigger);
		EnhancedInputComponent->BindAction(WeaponL1Action, ETriggerEvent::Completed, this, &AMyCharacter::WeaponL1_Complete);

		EnhancedInputComponent->BindAction(WeaponL2Action, ETriggerEvent::Started, this, &AMyCharacter::WeaponL2_Start);
		EnhancedInputComponent->BindAction(WeaponL2Action, ETriggerEvent::Triggered, this, &AMyCharacter::WeaponL2_Trigger);
		EnhancedInputComponent->BindAction(WeaponL2Action, ETriggerEvent::Canceled, this, &AMyCharacter::WeaponL2_Canceled);
		EnhancedInputComponent->BindAction(WeaponL2Action, ETriggerEvent::Completed, this, &AMyCharacter::WeaponL2_Complete);

		EnhancedInputComponent->BindAction(WeaponAbilityAction, ETriggerEvent::Triggered, this, &AMyCharacter::WeaponAbility);
		EnhancedInputComponent->BindAction(WeaponAbilityAction, ETriggerEvent::Completed, this, &AMyCharacter::WeaponAbility);

		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Completed, this, &AMyCharacter::Evade);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &AMyCharacter::Dodge);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMyCharacter::Interact);
	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	MoveInputValue = Value.Get<FVector2D>();

	Move(MoveInputValue);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}

	LookSideTarget(Value);
}

void AMyCharacter::LookSideTarget(const FInputActionValue& Value)
{
	FVector StartLocation = Camera->GetComponentLocation();
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (bLockOn && Value.GetMagnitude() >= 0.85f)
	{
		TArray<AEnemy*> List = FindInViewportEnemyList();

		if (!List.IsEmpty())
		{
			FVector Forward = GetActorForwardVector();
			FVector Pos = GetActorLocation();
			bool bLeft = LookAxisVector.X < 0;

			List.RemoveAll([&](const TObjectPtr<AEnemy>& Itr)
			{
				FVector Dir = (Itr->GetActorLocation() - Pos);

				Dir.Normalize();

				FVector Cross = FVector::CrossProduct(Forward, Dir);

				return TargetCharacter == Itr || bLeft ? Cross.Z > 0 : Cross.Z < 0;
			});

			if (!List.IsEmpty())
			{
				int FindIdx = List.IndexOfByPredicate([&](const TObjectPtr<AEnemy>& Itr) { return !IsOcclusionTo(StartLocation, Itr) && Itr->GetDistanceTo(this) <= MAX_LOCKON_DISTANCE; });

				if (List.IsValidIndex(FindIdx))
				{
					SetTargetCharacter(List[FindIdx]);
				}
			}
		}
	}
}

void AMyCharacter::LockOnMode()
{
	FVector StartLocation = Camera->GetComponentLocation();
	TObjectPtr<AEnemy> FindTarget = nullptr;
	TObjectPtr<UWorld> World = GetWorld();

	if(TargetCharacter == nullptr)
	{
		TArray<TObjectPtr<AEnemy>> List = FindInViewportEnemyList();

		if (!List.IsEmpty())
		{
			int FindIdx = List.IndexOfByPredicate([&](const TObjectPtr<AEnemy>& Itr) { return !IsOcclusionTo(StartLocation, Itr) && Itr->GetDistanceTo(this) <= MAX_LOCKON_DISTANCE; });

			if (List.IsValidIndex(FindIdx))
			{
				FindTarget = List[FindIdx];
			}
		}
	}

	SetTargetCharacter(FindTarget);
}

void AMyCharacter::OrientCamera()
{
	//rotation yaw
	Controller->SetControlRotation(GetActorRotation());
}

void AMyCharacter::Run(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	bool bRunMode = Value.Get<bool>();

	MovementState = StatusComponent->IsEnoughStamina(RunCost) && bRunMode ? MOVEMENT_STATE::RUN : MOVEMENT_STATE::WALK;

	CalcMoveSpeed();
}

void AMyCharacter::WeaponR1_Start(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	float ExecutionRange = 95;
	FVector BoxPos = GetActorLocation();
	FQuat BoxQuat = GetActorRotation().Quaternion();
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel
	(
		HitResult,
		BoxPos,
		BoxPos,
		BoxQuat,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(ExecutionRange),
		Params
	);

	//DrawDebugSphere(GetWorld(), BoxPos, ExecutionRange, 16, bResult ? FColor::Green : FColor::Red, false, 0.4f);

	if (bResult)
	{
		TObjectPtr<AGameUnit> Unit = nullptr;

		if (HitResult.GetActor()->IsValidLowLevel())
		{
			Unit = Cast<AGameUnit>(HitResult.GetActor());
		}

		if (Unit != nullptr && Unit->ActorHasTag(GetHostileUnitTag()) && Unit->GetStatus().PoisePoint <= 0)
		{
			HIT_DIRECTION_TYPE HitDirection = HIT_DIRECTION_TYPE::HIT_DIRECTION_TYPE_END;
			FVector Forward = Unit->GetActorForwardVector();
			FVector Dir = (BoxPos - Unit->GetActorLocation()).GetSafeNormal();
			float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, Dir)));

			if (0 <= Angle && Angle <= 70)
			{
				HitDirection = HIT_DIRECTION_TYPE::FORWARD;
			}
			else if (110 <= Angle && Angle <= 180)
			{
				HitDirection = HIT_DIRECTION_TYPE::BACKWARD;
			}

			if (HitDirection == HIT_DIRECTION_TYPE::FORWARD)
			{
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Unit->GetActorLocation());
				FRotator TargetAt = UKismetMathLibrary::FindLookAtRotation(Unit->GetActorLocation(), GetActorLocation());
				FRotator CharacterRot = FRotator(0, LookAt.Yaw, 0);
				FRotator TargetRot = FRotator(0, TargetAt.Yaw, 0);

				SetActorRotation(CharacterRot);
				Unit->SetActorRotation(TargetRot);

				PlayExecute(TEXT("E_F"), Unit);
				
				return;
			}
			else if (HitDirection == HIT_DIRECTION_TYPE::BACKWARD)
			{
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Unit->GetActorLocation());
				FRotator CharacterRot = FRotator(0, LookAt.Yaw, 0);

				SetActorRotation(CharacterRot);
				Unit->SetActorRotation(CharacterRot);

				PlayExecute(TEXT("E_B"), Unit);

				return;
			}
		}
	}

	if (EquipmentR != nullptr && EquipmentR->IsAvailableBehavior(StatusComponent, true))
	{
		EquipmentR->PlayStart();
	}
}

void AMyCharacter::WeaponR1_Trigger(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentR != nullptr)
	{
		EquipmentR->PlayTrigger();
	}
}

void AMyCharacter::WeaponR1_Complete(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentR != nullptr)
	{
		EquipmentR->PlayCompleted();
	}
}

void AMyCharacter::WeaponR2_Start(const FInputActionValue& Value)
{
	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
	{
		bWeaponAbility = true;
	}

	if (!IsAvailableBehavior() || !bWeaponAbility)
	{
		return;
	}

	if (EquipmentR != nullptr && EquipmentR->IsAvailableBehavior(StatusComponent, false))
	{
		EquipmentR->PlayChargeStart();
	}
}

void AMyCharacter::WeaponR2_Trigger(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentR != nullptr)
	{
		EquipmentR->PlayChargeTrigger();
	}
}

void AMyCharacter::WeaponR2_Canceled(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentR != nullptr)
	{
		EquipmentR->PlayChargeCanceled();
	}

	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
	{
		bWeaponAbility = false;
	}

}

void AMyCharacter::WeaponR2_Complete(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentR != nullptr)
	{
		EquipmentR->PlayChargeCompleted();
	}

	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
	{
		bWeaponAbility = false;
	}

}

void AMyCharacter::WeaponL1_Start(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentL != nullptr && EquipmentR->IsAvailableBehavior(StatusComponent, true))
	{
		EquipmentL->PlayStart();
	}
}

void AMyCharacter::WeaponL1_Trigger(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentL != nullptr)
	{
		EquipmentL->PlayTrigger();
	}
}

void AMyCharacter::WeaponL1_Complete(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentL != nullptr)
	{
		EquipmentL->PlayCompleted();
	}
}

void AMyCharacter::WeaponL2_Start(const FInputActionValue& Value)
{
	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
	{
		bWeaponAbility = true;
	}

	if (!IsAvailableBehavior() || !bWeaponAbility)
	{
		return;
	}

	if (EquipmentL != nullptr && EquipmentR->IsAvailableBehavior(StatusComponent, false))
	{
		EquipmentL->PlayChargeStart();
	}
}

void AMyCharacter::WeaponL2_Trigger(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentL != nullptr)
	{
		EquipmentL->PlayChargeTrigger();
	}
}

void AMyCharacter::WeaponL2_Canceled(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentL != nullptr)
	{
		EquipmentL->PlayChargeCanceled();
	}

	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
	{
		bWeaponAbility = false;
	}

}

void AMyCharacter::WeaponL2_Complete(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior())
	{
		return;
	}

	if (EquipmentL != nullptr)
	{
		EquipmentL->PlayChargeCompleted();
	}

	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
	{
		bWeaponAbility = false;
	}

}

void AMyCharacter::WeaponAbility(const FInputActionValue& Value)
{
	bWeaponAbility = Value.Get<bool>();
}

void AMyCharacter::Evade(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior() || !StatusComponent->IsEnoughStamina(EvadeCost))
	{
		return;
	}

	if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad && MoveInputValue.Length() <= 0.5f)
	{
		Dodge(Value);
		return;
	}

	if (EvadeState == 0)
	{
		EvadeState = 2;

		StatusComponent->AddStamina(-EvadeCost);

		GetCapsuleComponent()->SetCapsuleSize(34, 40);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -40));
		SpringArm->SetRelativeLocation(FVector(0, 0, 50));

		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(AnimMontageEvade, 1.0f);
			AnimInstance->Montage_JumpToSection(GetEvadeInputDirection(), AnimMontageEvade);
		}
	}
}

void AMyCharacter::Dodge(const FInputActionValue& Value)
{
	if (!IsAvailableBehavior() || !StatusComponent->IsEnoughStamina(EvadeCost))
	{
		return;
	}

	if (EvadeState == 0)
	{
		EvadeState = 1;

		StatusComponent->AddStamina(-EvadeCost);

		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(AnimMontageEvade, 1.0f);
			AnimInstance->Montage_JumpToSection("Dodge", AnimMontageEvade);
		}
	}
}

void AMyCharacter::Interact(const FInputActionValue& Value)
{
	if (GetPlayerUI()->IsVisibleDialogue())
	{
		GetPlayerUI()->HideDialogue();
		return;
	}

	if (CurrentInteractObject != nullptr)
	{
		CurrentInteractObject->Interact(this);
	}
}

TArray<TObjectPtr<AEnemy>> AMyCharacter::FindInViewportEnemyList()
{
	TArray<TObjectPtr<AEnemy>> ResultList;
	TObjectPtr<UWorld> World = GetWorld();
	FVector Pos = GetActorLocation();

	for (TActorIterator<AEnemy> Itr(World); Itr; ++Itr)
	{
		if (Itr->GetCharacterState() == CHARACTER_STATE::ALIVE && Itr->IsInViewport())
		{
			ResultList.Add(*Itr);
		}
	}

	ResultList.Sort([&](const AEnemy& Lhs, const AEnemy& Rhs)
	{
		float LDist = FVector::Dist(Lhs.GetActorLocation(), Pos);
		float RDist = FVector::Dist(Rhs.GetActorLocation(), Pos);

		return LDist > RDist;
	});


	return ResultList;
}

TTuple<FVector, FVector> AMyCharacter::GetMovementDirection()
{
	FRotator Rotation = Controller->GetControlRotation();

	if (bLockOn && MovementState == MOVEMENT_STATE::WALK)
	{
		Rotation = GetActorRotation();
	}

	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	return TTuple<FVector, FVector>(ForwardDirection, RightDirection);
}

void AMyCharacter::Move(FVector2D MovementVector)
{
	if (Controller != nullptr)
	{
		const TTuple<FVector, FVector> Direction = GetMovementDirection();
		const FVector ForwardDirection = Direction.Get<0>();
		const FVector RightDirection = Direction.Get<1>();

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyCharacter::ResetState()
{
	MovementState = MOVEMENT_STATE::WALK;
	EvadeState = 0;

	SetHitReaction(false);
	CalcMoveSpeed();
}

FName AMyCharacter::GetEvadeInputDirection()
{
	FName EvadeName = TEXT("Evade_F");

	if (bLockOn && MovementState == MOVEMENT_STATE::WALK)
	{
		FVector ForwardDirection = GetMovementDirection().Get<0>();
		FVector InputDirection(MoveInputValue.Y, MoveInputValue.X, 0);
		FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (MyGameConfig::GetMostRecentlyHardwareDeviceType(this) == EHardwareDevicePrimaryType::Gamepad)
		{
			InputDirection.Normalize();
		}

		if (TargetCharacter != nullptr)
		{
			ForwardDirection = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}

		InputDirection = YawRotation.RotateVector(InputDirection);

		FVector Cross = FVector::CrossProduct(ForwardDirection, InputDirection);
		float CAngle = ForwardDirection.CosineAngle2D(InputDirection);
		float Angle = FMath::RadiansToDegrees(FMath::Acos(CAngle));
		bool bRight = Cross.Z > 0;

		if (0 <= Angle && Angle < 5)
		{
			EvadeName = TEXT("Evade_F");
		}
		else if (5 <= Angle && Angle < 70)
		{
			EvadeName = bRight ? TEXT("Evade_FR") : TEXT("Evade_FL");
		}
		else if (70 <= Angle && Angle < 110)
		{
			EvadeName = bRight ? TEXT("Evade_R") : TEXT("Evade_L");
		}
		else if (110 <= Angle && Angle < 175)
		{
			EvadeName = bRight ? TEXT("Evade_BR") : TEXT("Evade_BL");
		}
		else if (175 <= Angle && Angle <= 180)
		{
			EvadeName = TEXT("Evade_B");
		}

		//UE_LOG(LogTemp, Log, TEXT("%s, forward : %.2f %.2f %.2f, input : %.2f %.2f %.2f, angle : %.2f, cross : %.2f %.2f %.2f"), *EvadeName.ToString(), ForwardDirection.X, ForwardDirection.Y, ForwardDirection.Z, InputDirection.X, InputDirection.Y, InputDirection.Z, Angle, Cross.X, Cross.Y, Cross.Z);
	}

	return EvadeName;
}

void AMyCharacter::SetOverlapInteractObject(TObjectPtr<AInteractObject> InteractObject)
{
	if (CurrentInteractObject != InteractObject)
	{
		CurrentInteractObject = InteractObject;
	}

	//UE_LOG(LogTemp, Log, TEXT("interact, %s"), CurrentInteractObject  != nullptr ? *CurrentInteractObject->GetActorNameOrLabel() : TEXT("none"));
}

void AMyCharacter::CalcMoveSpeed()
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

	bool bOrient = !bLockOn || MovementState == MOVEMENT_STATE::RUN;

	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	GetCharacterMovement()->MaxAcceleration = NewSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = bOrient;
	GetCharacterMovement()->bUseControllerDesiredRotation = !bOrient;
}

void AMyCharacter::CheckShield()
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

FName AMyCharacter::GetHostileUnitTag()
{
	return TEXT("Game.Unit.Enemy");
}

bool AMyCharacter::IsAvailableBehavior()
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

void AMyCharacter::PlayHitReaction(const FName& SectionName, float PlayRate)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimMontageHit->GetSectionIndex(SectionName) != INDEX_NONE)
	{
		AnimInstance->Montage_Play(AnimMontageHit, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontageHit);
	}
}

bool AMyCharacter::IsEquipmentContinue(EQUIP_HAND_TYPE HandType)
{
	if (HandType == EQUIP_HAND_TYPE::RIGHT_HAND && EquipmentR != nullptr)
	{
		return EquipmentR->IsContinue();
	}
	else if (HandType == EQUIP_HAND_TYPE::LEFT_HAND && EquipmentL != nullptr)
	{
		return EquipmentL->IsContinue();
	}

	return false;
}

bool AMyCharacter::IsEquipmentContinueCharge(EQUIP_HAND_TYPE HandType)
{
	if (HandType == EQUIP_HAND_TYPE::RIGHT_HAND && EquipmentR != nullptr)
	{
		return EquipmentR->IsContinueCharge();
	}
	else if (HandType == EQUIP_HAND_TYPE::LEFT_HAND && EquipmentL != nullptr)
	{
		return EquipmentL->IsContinueCharge();
	}

	return false;
}

void AMyCharacter::ComboAttack(EQUIP_HAND_TYPE HandType, const FString& NextSection)
{
	if (HandType == EQUIP_HAND_TYPE::RIGHT_HAND && EquipmentR != nullptr)
	{
		EquipmentR->PlayCombo(NextSection);
	}
	else if (HandType == EQUIP_HAND_TYPE::LEFT_HAND && EquipmentL != nullptr)
	{
		EquipmentL->PlayCombo(NextSection);
	}
}

void AMyCharacter::ComboChargeAttack(EQUIP_HAND_TYPE HandType)
{
	if (HandType == EQUIP_HAND_TYPE::RIGHT_HAND && EquipmentR != nullptr)
	{
		EquipmentR->PlayChargeCombo();
	}
	else if (HandType == EQUIP_HAND_TYPE::LEFT_HAND && EquipmentL != nullptr)
	{
		EquipmentL->PlayChargeCombo();
	}
}

void AMyCharacter::AttackEnd(EQUIP_HAND_TYPE HandType, float BlendOut)
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

void AMyCharacter::ChargeAttackEnd(EQUIP_HAND_TYPE HandType, bool bInitState)
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

void AMyCharacter::OnTargeted(bool bTargeted)
{
	SetForceWaitState(false);
}

void AMyCharacter::OnStagger()
{
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController != nullptr)
	{
		DisableInput(PlayerController);
	}

	SetHitReaction(true);
	AttackEnd(EQUIP_HAND_TYPE::RIGHT_HAND, 1.0f);
	AttackEnd(EQUIP_HAND_TYPE::LEFT_HAND, 1.0f);
	PlayHitReaction(TEXT("GB"), 0.25f);
}

void AMyCharacter::SetTargetCharacter(TObjectPtr<AGameUnit> Unit)
{
	if (CharacterState == CHARACTER_STATE::DEATH)
	{
		return;
	}

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->OnTargeted(false);
		PlayerUI->ClearBossHUD(TargetCharacter);
	}

	TargetCharacter = Unit;

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->OnTargeted(true);
		PlayerUI->SetBossHUD(TargetCharacter);
	}

	bLockOn = TargetCharacter != nullptr;
	SpringArm->bUsePawnControlRotation = !bLockOn;

	float SpringArmLag = bLockOn ? 8.0f : 15.0f;
	float SpringArmRotLag = bLockOn ? 11.0f : 15.0f;

	SpringArm->CameraLagSpeed = SpringArmLag;
	SpringArm->CameraRotationLagSpeed = SpringArmRotLag;

	PlayerUI->OnOffLockOnHUD(bLockOn);
}

void AMyCharacter::Damaged(float Damage, AGameUnit* DamageCauser)
{
	if (CharacterState == CHARACTER_STATE::DEATH || Damage <= 0)
	{
		return;
	}

	FName SectionName = "";
	float PlayRate = 1.0f;
	bool bDamage = false;
	bool bForwardHit = MyGameConfig::IsForwardHitDirection(DamageCauser, this);
	FCharacterStatus Status = GetStatus();
	
	if (bExecutionMotion)
	{
		bDamage = StatusComponent->ExecutionDamaged(Damage, DamageCauser);
	}
	else
	{
		bDamage = StatusComponent->Damaged(Damage, DamageCauser);
	}

	if (bDamage)
	{
		if (Status.Hp <= 0)
		{
			Death();
		}
		else if(Status.Hp > 0 && bHitReaction)
		{
			int Rnd = FMath::RandRange(1, 2);

			SectionName = *FString::Printf((bForwardHit ? TEXT("FH%d") : TEXT("BH%d")), Rnd);
		}
	}
	else
	{
		if (Status.Stamina > 0)
		{
			//guard reaction
			SectionName = "GH1";
		}
	}

	if (Status.PoisePoint <= 0)
	{
		//guard break, stagger
		SectionName = "GB1";
		PlayRate = 0.25f;
	}

	if (SectionName != "" && !bExecutionMotion)
	{
		if (EquipmentR != nullptr && EquipmentR->GetType() != EQUIPMENT_TYPE::SHIELD)
		{
			AttackEnd(EQUIP_HAND_TYPE::RIGHT_HAND, 1.0f);
		}

		if (EquipmentL != nullptr && EquipmentL->GetType() != EQUIPMENT_TYPE::SHIELD)
		{
			AttackEnd(EQUIP_HAND_TYPE::LEFT_HAND, 1.0f);
		}

		PlayHitReaction(SectionName, PlayRate);
	}
}

void AMyCharacter::Death()
{
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller);

	CharacterState = CHARACTER_STATE::DEATH;

	DisableInput(PlayerController);
	SetActorEnableCollision(false);

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->SetTargetCharacter(nullptr);
	}

	PlayerUI->OnOffLockOnHUD(false);
	PlayerUI->PlayDieHUDAnim();
}

void AMyCharacter::HitRecover()
{
	FCharacterStatus Status = GetStatus();
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController != nullptr && !InputEnabled())
	{
		EnableInput(PlayerController);
	}

	ResetState();

	if (Status.Hp > 0)
	{
		CharacterState = CHARACTER_STATE::ALIVE;
	}
	else
	{
		Death();
	}
}

void AMyCharacter::ParryEnd()
{
	if (EquipmentL != nullptr && EquipmentL->GetType() == EQUIPMENT_TYPE::SHIELD)
	{
		EquipmentL->ChargeEnd();
	}
	else if (EquipmentR != nullptr && EquipmentR->GetType() == EQUIPMENT_TYPE::SHIELD)
	{
		EquipmentR->ChargeEnd();
	}
}

void AMyCharacter::EvadeEnd()
{
	ResetState();
	GetCapsuleComponent()->SetCapsuleSize(34, 90);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	SpringArm->SetRelativeLocation(FVector(0, 0, 0));
}


