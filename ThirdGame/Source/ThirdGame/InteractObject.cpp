// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractObject.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"

// Sets default values
AInteractObject::AInteractObject()
{
	InteractRange = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	InteractRange->SetSphereRadius(300);
	//InteractRange->bHiddenInGame = false; //test

	SetRootComponent(InteractRange);

	Delegate_BeginOverlap.BindUFunction(this, TEXT("BeginOverlap"));
	Delegate_EndOverlap.BindUFunction(this, TEXT("EndOverlap"));
	InteractRange->OnComponentBeginOverlap.Add(Delegate_BeginOverlap);
	InteractRange->OnComponentEndOverlap.Add(Delegate_EndOverlap);
}

// Called when the game starts or when spawned
void AInteractObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractObject::BeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);
	TObjectPtr<UCapsuleComponent> PlayerCapsule = Cast<UCapsuleComponent>(OtherComponent);

	if (Player != nullptr && PlayerCapsule != nullptr)
	{
		SetInteractable(true);
		OnChangedInteractble(Player);
		Player->SetOverlapInteractObject(this);
	}
}


void AInteractObject::EndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);
	TObjectPtr<UCapsuleComponent> PlayerCapsule = Cast<UCapsuleComponent>(OtherComponent);

	if (Player != nullptr && PlayerCapsule != nullptr)
	{
		SetInteractable(false);
		OnChangedInteractble(Player);
		Player->SetOverlapInteractObject(nullptr);
	}
}
