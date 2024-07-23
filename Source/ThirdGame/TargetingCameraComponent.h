// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TargetingCameraComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AGameUnit;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDGAME_API UTargetingCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingCameraComponent();

	FVector GetCameraLocation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
private:
	bool bLockOnMode = false;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AGameUnit> TargetCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> Camera = nullptr;
};
