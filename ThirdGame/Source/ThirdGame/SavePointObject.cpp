// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePointObject.h"
#include "MyCharacter.h"
#include "PlayerUI.h"
#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"

ASavePointObject::ASavePointObject()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	Mesh->SetupAttachment(RootComponent);
}

void ASavePointObject::Interact(TObjectPtr<AMyCharacter> Player)
{
	if (SavePoint())
	{
		FString Text = TEXT("Success Save!");

		Player->GetPlayerUI()->Dialogue(Text);
	}
}

void ASavePointObject::BeginPlay()
{
	Super::BeginPlay();
}

void ASavePointObject::OnChangedInteractble(TObjectPtr<AMyCharacter> Player)
{
	Player->GetPlayerUI()->OnOffAcceptUI(bInteractable);
}
