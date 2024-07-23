// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleDialogue.h"
#include "Components/TextBlock.h"

void USimpleDialogue::Dialogue(const FString& Text, float Duration)
{
	if (DialogueText != nullptr)
	{
		DialogueText->SetText(FText::FromString(Text));
	}

	SetVisibility(ESlateVisibility::Visible);
	StopTimer();
	StartTimer(Duration);
}

void USimpleDialogue::NativeOnInitialized()
{
	DialogueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
}

void USimpleDialogue::StartTimer(float Duration)
{
	TObjectPtr<UWorld> World = GetWorld();

	if (World != nullptr && !World->GetTimerManager().TimerExists(AutoDisableTimer))
	{
		World->GetTimerManager().SetTimer(AutoDisableTimer, this, &USimpleDialogue::HideWidget, Duration, false);
	}
}

void USimpleDialogue::StopTimer()
{
	TObjectPtr<UWorld> World = GetWorld();

	if (World != nullptr && World->GetTimerManager().TimerExists(AutoDisableTimer))
	{
		World->GetTimerManager().ClearTimer(AutoDisableTimer);
	}
}

void USimpleDialogue::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}
