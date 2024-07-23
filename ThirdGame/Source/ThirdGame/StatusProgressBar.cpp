// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusProgressBar.h"

void UStatusProgressBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Overlay = Cast<UOverlay>(GetWidgetFromName(TEXT("Overlay")));
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	ProgressText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ProgressText")));
}

void UStatusProgressBar::SetPercent(float Value, float MaxValue)
{
	if (ProgressBar != nullptr)
	{
		float Ratio = Value / MaxValue;

		ProgressBar->SetPercent(Ratio);
	}

	if (ProgressText != nullptr && ProgressText->GetVisibility() == ESlateVisibility::Visible)
	{
		SetText(Value, MaxValue);
	}
}

void UStatusProgressBar::SetText(float Value, float MaxValue)
{
	if (ProgressText != nullptr)
	{
		FString RatioText = FString::Printf(TEXT("%.0f / %.0f"), Value, MaxValue);

		ProgressText->SetText(FText::FromString(RatioText));
	}
}

void UStatusProgressBar::SetVisibleText(bool bVisible)
{
	if (ProgressText != nullptr)
	{
		ESlateVisibility Visible = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

		ProgressText->SetVisibility(Visible);
	}
}

void UStatusProgressBar::SetProgressBarFillColor(FLinearColor Color)
{
	FillColor = Color;

	ProgressBar->SetFillColorAndOpacity(FillColor);
}
