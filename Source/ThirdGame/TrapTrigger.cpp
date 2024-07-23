// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapTrigger.h"
#include "Components/BoxComponent.h"
#include "GameUnit.h"
#include "MyCharacter.h"

// Sets default values
ATrapTrigger::ATrapTrigger()
{
	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));

	Delegate_BeginOverlap.BindUFunction(this, TEXT("BeginOverlap"));
	Delegate_EndOverlap.BindUFunction(this, TEXT("EndOverlap"));
	TriggerArea->OnComponentBeginOverlap.Add(Delegate_BeginOverlap);
	TriggerArea->OnComponentEndOverlap.Add(Delegate_EndOverlap);

	//TriggerArea->bHiddenInGame = false; //test
}

// Called when the game starts or when spawned
void ATrapTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	for (auto& Itr : LinkedTriggerList)
	{
		if (Itr.Key != nullptr)
		{
			Itr.Key->SetForceWaitState(true);

			if (Itr.Value != TEXT(""))
			{
				Delegate_Trigger.AddUFunction(Itr.Key, *Itr.Value);
			}
		}
	}
}

void ATrapTrigger::BeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);

	if (Player == nullptr)
	{
		return;
	}

	Delegate_Trigger.Broadcast();

	for (auto& Itr : LinkedTriggerList)
	{
		if (Itr.Key != nullptr && Itr.Key->GetForceWaitState())
		{
			if (Delegate_Trigger.IsBound())
			{
				Itr.Key->SetForceWaitState(false);
				Itr.Key->SetTargetCharacter(Player);
			}
		}
	}

	Delegate_Trigger.Clear();
	Delegate_BeginOverlap.Clear();
	Delegate_EndOverlap.Clear();
}

void ATrapTrigger::EndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);

	if (Player == nullptr)
	{
		return;
	}
}
