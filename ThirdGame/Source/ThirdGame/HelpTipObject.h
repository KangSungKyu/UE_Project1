// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractObject.h"
#include "HelpTipObject.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API AHelpTipObject : public AInteractObject
{
	GENERATED_BODY()

public:
	AHelpTipObject();

	virtual void Interact(TObjectPtr<AMyCharacter> Player) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnChangedInteractble(TObjectPtr<AMyCharacter> Player) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(EditAnywhere)
	FString TipMessage = TEXT("");
};
