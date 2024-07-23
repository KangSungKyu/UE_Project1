// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameUnit.h"

// Sets default values for this component's properties
UTargetingCameraComponent::UTargetingCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bLockOnMode = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	//SpringArm->SetupAttachment(this);
	//Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	SpringArm->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);

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
	// ...
}

FVector UTargetingCameraComponent::GetCameraLocation()
{
	return Camera->GetComponentLocation();
}


// Called when the game starts
void UTargetingCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTargetingCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

