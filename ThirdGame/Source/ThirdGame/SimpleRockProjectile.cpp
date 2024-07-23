// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRockProjectile.h"
#include "Components/SphereComponent.h"
#include "MyGameConfig.h"
#include "GameUnit.h"
#include "EquipmentComponent.h"
#include "Kismet/GameplayStatics.h"

ASimpleRockProjectile::ASimpleRockProjectile()
{
	AutoDisableTime = 5.5f;
}

void ASimpleRockProjectile::InitProjectile(const FVector& Location, const FRotator& Rotator)
{
	TObjectPtr<UStaticMesh> TempMesh = MyGameConfig::LoadObject<UStaticMesh>(TEXT("StaticMesh'/Game/StarterContent/Props/SM_Rock.SM_Rock'"));

	ShapeComponent = ::NewObject<UShapeComponent>(this, USphereComponent::StaticClass(), TEXT("MeshCollide"));
	ShapeComponent->RegisterComponent();
	PMComponent = ::NewObject<UProjectileMovementComponent>(this, UProjectileMovementComponent::StaticClass(), TEXT("ProjectileMovementComponent"));
	PMComponent->RegisterComponent();

	TObjectPtr<USphereComponent> Sphere = Cast<USphereComponent>(ShapeComponent);

	SetRootComponent(Sphere);

	//Sphere->bHiddenInGame = false; //test
	Sphere->InitSphereRadius(7.5);
	Sphere->SetWorldLocationAndRotation(Location, Rotator);
	Sphere->BodyInstance.SetCollisionProfileName(TEXT("Throwable"));
	Sphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	Sphere->OnComponentHit.Add(Delegate_OnHit);

	PMComponent->SetUpdatedComponent(ShapeComponent);

	if (TempMesh != nullptr)
	{
		ProjectileMesh->SetStaticMesh(TempMesh);
		ProjectileMesh->SetRelativeLocationAndRotation(FVector(0, 0, -5), FRotator(0, 0, 0));
		ProjectileMesh->SetRelativeScale3D(FVector::OneVector * 0.05f);
		ProjectileMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	PMComponent->InitialSpeed = 175.0f;
	PMComponent->MaxSpeed = 1000.0f;
	PMComponent->bRotationFollowsVelocity = true;
	PMComponent->bShouldBounce = false;
	PMComponent->Bounciness = 0.0f;
	PMComponent->ProjectileGravityScale = 0.9f;
	PMComponent->Friction = 0.0f;
}

void ASimpleRockProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	TObjectPtr<AGameUnit> OwnerCharacter = FireEquipment->GetOwnerCharacter();
	TObjectPtr<AGameUnit> Unit = Cast<AGameUnit>(OtherActor);

	if (Unit != nullptr && OwnerCharacter->GetHostileUnitTag() == Unit->GetUnitTag())
	{
		UGameplayStatics::ApplyDamage(Unit, OwnerCharacter->GetStatus().Attack, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
		DeleteProjectile();
	}
	else
	{
		SetDisableTimer();
	}
}
