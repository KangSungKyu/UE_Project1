// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileObject.h"
#include "EquipmentComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ObjectPoolSubsystem.h"
#include "SimpleRockProjectile.h"

TObjectPtr<AProjectileObject> AProjectileObject::Create(TObjectPtr<USceneComponent> AttachRoot, TObjectPtr<UEquipmentComponent> Equipment, const FVector& Location, const FRotator& Rotator, TSubclassOf<AProjectileObject> SubClass)
{
	TObjectPtr<UWorld> World = AttachRoot->GetWorld();

	TObjectPtr<AProjectileObject> Projectile = nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Equipment->GetOwner();

	TObjectPtr<AActor> FromPool = World->GetGameInstance()->GetSubsystem<UObjectPoolSubsystem>()->Get(SubClass);
	Projectile = Cast<AProjectileObject>(FromPool);

	if (Projectile == nullptr)
	{
		Projectile = World->SpawnActor<AProjectileObject>(SubClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}

	Projectile->ProjectileMesh = ::NewObject<UStaticMeshComponent>(Projectile, UStaticMeshComponent::StaticClass(), TEXT("StaticMesh"));
	Projectile->ProjectileMesh->RegisterComponent();

	Projectile->FireEquipment = Equipment;
	Projectile->Delegate_OnHit.BindUFunction(Projectile, TEXT("OnHit"));

	Projectile->InitProjectile(Location, Rotator);

	return Projectile;
}

// Sets default values
AProjectileObject::AProjectileObject()
{
	FireEquipment = nullptr;
	ProjectileMesh = nullptr;
	ShapeComponent = nullptr;
	PMComponent = nullptr;
}

void AProjectileObject::FireWithArc(const FVector& TargetLocation, float Arc)
{
	if (PMComponent != nullptr)
	{
		TObjectPtr<UWorld> World = GetWorld();
		FVector Velocity;

		UGameplayStatics::SuggestProjectileVelocity_CustomArc(World, Velocity, GetActorLocation(), TargetLocation, PMComponent->GetGravityZ(), Arc);

		PMComponent->AddForce(Velocity * PMComponent->InitialSpeed);

	}
}

void AProjectileObject::SetDisableTimer()
{
	TObjectPtr<UWorld> World = GetWorld();

	if (World != nullptr && !World->GetTimerManager().TimerExists(AutoDisableTimer))
	{
		World->GetTimerManager().SetTimer(AutoDisableTimer, this, &AProjectileObject::DeleteProjectile, AutoDisableTime, false);
	}
}

void AProjectileObject::DeleteProjectile()
{
	TObjectPtr<UWorld> World = GetWorld();
	
	if (World != nullptr)
	{
		if (World->GetTimerManager().TimerExists(AutoDisableTimer))
		{
			World->GetTimerManager().ClearTimer(AutoDisableTimer);
		}

		//Destroy();
		World->GetGameInstance()->GetSubsystem<UObjectPoolSubsystem>()->Release(StaticClass(), this);
	}
}
