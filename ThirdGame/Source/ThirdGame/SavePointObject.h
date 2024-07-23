// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractObject.h"
#include "SavePointObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API ASavePointObject : public AInteractObject
{
	GENERATED_BODY()

public:
	ASavePointObject();

	virtual void Interact(TObjectPtr<AMyCharacter> Player) override;

	UFUNCTION(BlueprintImplementableEvent)
	bool SavePoint();

protected:
	virtual void BeginPlay() override;
	virtual void OnChangedInteractble(TObjectPtr<AMyCharacter> Player) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString SavePointName = TEXT("");
};
