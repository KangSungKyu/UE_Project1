// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyRegion.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyRegion::AEnemyRegion()
{
	Region = CreateDefaultSubobject<USphereComponent>(TEXT("Region"));

	Region->SetSphereRadius(500.0f);

	Delegate_BeginOverlap.BindUFunction(this, TEXT("BeginOverlap"));
	Delegate_EndOverlap.BindUFunction(this, TEXT("EndOverlap"));
	Region->OnComponentBeginOverlap.Add(Delegate_BeginOverlap);
	Region->OnComponentEndOverlap.Add(Delegate_EndOverlap);
}

float AEnemyRegion::GetRegionRadius()
{
	return Region->GetScaledSphereRadius();
}

void AEnemyRegion::LinkEnemy(const TObjectPtr<AEnemy>& Enemy)
{
	if (!LinkedEnemyList.Contains(Enemy))
	{
		LinkedEnemyList.Add(Enemy);
	}
}


void AEnemyRegion::BeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//nothing
	if (Cast<UCapsuleComponent>(OtherComponent) != nullptr)
	{
		TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);

		if (Player != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("begin overlap, enemy region, other %s"), *Player->GetName());
		}
	}
}

void AEnemyRegion::EndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if(Cast<UCapsuleComponent>(OtherComponent) == nullptr)
	{
		return;
	}

	TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);

	if (Player != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("end overlap, enemy region, other %s"), *Player->GetName());

		//enemy's target reset, go home
		for (auto& Itr : LinkedEnemyList)
		{
			Itr->GoHome();
		}
	}
	else
	{
		TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy != nullptr && LinkedEnemyList.Contains(Enemy))
		{
			Enemy->GoHome();
		}
	}
}
