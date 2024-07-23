// Fill out your copyright notice in the Description page of Project Settings.


#include "HelpTipObject.h"
#include "MyCharacter.h"
#include "PlayerUI.h"

AHelpTipObject::AHelpTipObject()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	Mesh->SetupAttachment(RootComponent);
}

void AHelpTipObject::BeginPlay()
{
	Super::BeginPlay();
}

void AHelpTipObject::OnChangedInteractble(TObjectPtr<AMyCharacter> Player)
{
	Player->GetPlayerUI()->OnOffAcceptUI(bInteractable);
}

void AHelpTipObject::Interact(TObjectPtr<AMyCharacter> Player)
{
	Player->GetPlayerUI()->Dialogue(TipMessage);
	//UE_LOG(LogTemp, Log, TEXT("%s"), *TipMessage);
}
