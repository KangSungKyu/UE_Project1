// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractObject.generated.h"

class USphereComponent;
class AMyCharacter;

UCLASS(Abstract, BlueprintType)
class THIRDGAME_API AInteractObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractObject();

	void SetInteractable(bool bValue) { bInteractable = bValue; }
	bool IsInteractable() { return bInteractable; }

	virtual void Interact(TObjectPtr<AMyCharacter> Player) PURE_VIRTUAL(AInteractObject::Interact, );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnChangedInteractble(TObjectPtr<AMyCharacter> Player) PURE_VIRTUAL(AInteractObject::OnChangedInteractble, );

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

protected:
	bool bInteractable = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> InteractRange = nullptr;

	FScriptDelegate Delegate_BeginOverlap;
	FScriptDelegate Delegate_EndOverlap;
};
