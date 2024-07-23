// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "StatusProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class THIRDGAME_API UStatusProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPercent(float Value, float MaxValue);
	void SetText(float Value, float MaxValue);
	void SetVisibleText(bool bVisible);
	void SetProgressBarFillColor(FLinearColor Color);
	FLinearColor GetProgressBarFillColor() { return FillColor; }

protected:
	virtual void NativeOnInitialized() override;
	
protected:
	TObjectPtr<UOverlay> Overlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProgressBar> ProgressBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> ProgressText = nullptr;

	UPROPERTY(EditAnywhere)
	FLinearColor FillColor = FLinearColor::White;
};
