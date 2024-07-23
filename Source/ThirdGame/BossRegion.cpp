// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRegion.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "Enemy.h"
#include "MyCharacter.h"
#include "EmptyActor.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABossRegion::ABossRegion()
{
	bPlayedEntrance = false;
	Region = CreateDefaultSubobject<USphereComponent>(TEXT("Region"));
	CenterOfRegionMeshes = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CenterRegion"));

	SetRootComponent(Region);
	CenterOfRegionMeshes->SetupAttachment(RootComponent);
	Region->SetSphereRadius(500.0f);
}

void ABossRegion::BeginPlay()
{
	Super::BeginPlay();

	if (CenterOfRegionMeshes != nullptr)
	{
		CenterOfRegionMeshes->SetVisibility(bPlayedEntrance, true);

		int WallCount = CenterOfRegionMeshes->GetNumChildrenComponents();

		for (int i = 0; i < WallCount; ++i)
		{
			TObjectPtr<UStaticMeshComponent> Child = Cast<UStaticMeshComponent>(CenterOfRegionMeshes->GetChildComponent(i));

			if (Child != nullptr)
			{
				Child->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

float ABossRegion::GetRegionRadius()
{
	return Region->GetScaledSphereRadius();
}

void ABossRegion::OnBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(OtherActor);

	if (Player != nullptr)
	{
		if (!bPlayedEntrance)
		{
			bPlayedEntrance = true;

			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			FVector Start = GetTransform().TransformPosition(EnterPosition);
			FNavLocation NavLocation;

			if (NavSystem->GetRandomPointInNavigableRadius(Start, 1.0f, NavLocation))
			{
				Player->SetActorLocation(NavLocation.Location);
			}

			if (CenterOfRegionMeshes != nullptr)
			{
				CenterOfRegionMeshes->SetVisibility(true, true);
				CenterOfRegionMeshes->SetRelativeLocation(FVector::ZeroVector);

				int WallCount = CenterOfRegionMeshes->GetNumChildrenComponents();

				for (int i = 0; i < WallCount; ++i)
				{
					TObjectPtr<UStaticMeshComponent> Child = Cast<UStaticMeshComponent>(CenterOfRegionMeshes->GetChildComponent(i));

					if (Child != nullptr)
					{
						Child->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					}
				}
			}
		}

		for (auto& Itr : LinkedEnemyList)
		{
			Itr->SetForceWaitState(false);
		}
	}
}

void ABossRegion::OnEndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
}
