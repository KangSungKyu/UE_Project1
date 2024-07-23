// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimpleDialogue.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API USimpleDialogue : public UUserWidget
{
	GENERATED_BODY()

public:
	void Dialogue(const FString& Text, float Duration = 2.0f);
	void HideWidget();

protected:
	virtual void NativeOnInitialized() override;

private:
	void StartTimer(float Duration);
	void StopTimer();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTextBlock> DialogueText = nullptr;

	FTimerHandle AutoDisableTimer;
};
