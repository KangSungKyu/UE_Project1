// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"
#include "GameUnit.h"
#include "MyCharacter.h"
#include "Enemy.h"
#include "CharacterStatusComponent.h"
#include "StatusProgressBar.h"
#include "Components/CanvasPanelSlot.h"
#include "SimpleDialogue.h"
#include "CommonUserWidget.h"
#include "UMG.h"


void UPlayerUI::Dialogue(const FString& Text, float Duration)
{
	if (SimpleDialogue != nullptr)
	{
		SimpleDialogue->Dialogue(Text, Duration);
	}
}

void UPlayerUI::OnOffAcceptUI(bool bValue)
{
	if (AcceptUI != nullptr)
	{
		AcceptUI->SetVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

bool UPlayerUI::IsVisibleDialogue()
{
	if (SimpleDialogue != nullptr)
	{
		return SimpleDialogue->GetVisibility() == ESlateVisibility::Visible;
	}

	return false;
}

void UPlayerUI::HideDialogue()
{
	if (SimpleDialogue != nullptr)
	{
		SimpleDialogue->HideWidget();
	}
}

void UPlayerUI::SetBossHUD(TObjectPtr<AGameUnit> Target)
{
	TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(Target);

	if (Enemy != nullptr && Enemy->GetUnitSizeType() == UNIT_SIZE_TYPE::BOSS)
	{
		CurrentBossHUDHandle = Enemy->GetStatusComponent()->Delegate_ChangeHP.AddUObject(this, &UPlayerUI::OnChangedBossHP);

		Enemy->GetStatusComponent()->Delegate_ChangeHP.Broadcast(Enemy->GetStatus().Hp, Enemy->GetStatus().MaxHp);
		Enemy->SetVisibleHPBar(false);
		Enemy->SetBossHUD(true);

		BossHPBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		BossHPBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerUI::ClearBossHUD(TObjectPtr<AGameUnit> Target)
{
	TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(Target);

	if (Enemy != nullptr && Enemy->GetUnitSizeType() == UNIT_SIZE_TYPE::BOSS && CurrentBossHUDHandle.IsValid())
	{
		Enemy->GetStatusComponent()->Delegate_ChangeHP.Remove(CurrentBossHUDHandle);
		Enemy->SetBossHUD(false);
	}

	BossHPBar->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerUI::OnOffLockOnHUD(bool bValue)
{
	LockOnHUD->SetVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UPlayerUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Player = Cast<AMyCharacter>(GetOwningPlayerPawn());

	PlayerHPBar = Cast<UStatusProgressBar>(GetWidgetFromName(TEXT("PlayerHPBar2")));
	PlayerStaminaBar = Cast<UStatusProgressBar>(GetWidgetFromName(TEXT("PlayerStaminaBar2")));
	BossHPBar = Cast<UStatusProgressBar>(GetWidgetFromName(TEXT("BossHPBar")));
	SimpleDialogue = Cast<USimpleDialogue>(GetWidgetFromName(TEXT("WBP_Dialog")));
	AcceptUI = Cast<UCommonUserWidget>(GetWidgetFromName(TEXT("WBP_InteractButton")));
	LockOnHUD = Cast<UImage>(GetWidgetFromName(TEXT("LockOnHUD")));


	PlayerHPBar->SetProgressBarFillColor(PlayerHPBar->GetProgressBarFillColor());
	PlayerStaminaBar->SetProgressBarFillColor(PlayerStaminaBar->GetProgressBarFillColor());
	BossHPBar->SetProgressBarFillColor(BossHPBar->GetProgressBarFillColor());

	if (Player != nullptr)
	{
		Player->SetPlayerUI(this);
		Player->GetStatusComponent()->Delegate_ChangeHP.AddUObject(this, &UPlayerUI::OnChangedPlayerHP);
		Player->GetStatusComponent()->Delegate_ChangeMaxHP.AddUObject(this, &UPlayerUI::SetPlayerHPBarSize);
		Player->GetStatusComponent()->Delegate_ChangeStamina.AddUObject(this, &UPlayerUI::OnChangedPlayerStamina);
		Player->GetStatusComponent()->Delegate_ChangeMaxStamina.AddUObject(this, &UPlayerUI::SetPlayerStaminaBarSize);
	}

	SimpleDialogue->HideWidget();
	BossHPBar->SetVisibility(ESlateVisibility::Hidden);
	
	OnOffAcceptUI(false);
	OnOffLockOnHUD(false);
}

void UPlayerUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Player != nullptr && Player->GetTargetCharacter() != nullptr)
	{
		UpdateLockOnHUDLocation(Player->GetTargetCharacter());
	}
}

void UPlayerUI::SetPlayerHPBarSize(float MaxHP)
{
	float Ratio = MaxHP / DefaultHPBarSize;
	TObjectPtr<UCanvasPanelSlot> SlotInfo = Cast<UCanvasPanelSlot>(PlayerHPBar->Slot);
	FVector2D Size = SlotInfo->GetSize();

	SlotInfo->SetSize(FVector2D(DefaultHPBarSize * Ratio, Size.Y));
}

void UPlayerUI::SetPlayerStaminaBarSize(float MaxStamina)
{
	float Ratio = MaxStamina / DefaultStaminaSize;
	TObjectPtr<UCanvasPanelSlot> SlotInfo = Cast<UCanvasPanelSlot>(PlayerStaminaBar->Slot);
	FVector2D Size = SlotInfo->GetSize();

	SlotInfo->SetSize(FVector2D(DefaultStaminaSize * Ratio, Size.Y));
}

void UPlayerUI::OnChangedPlayerHP(float HP, float MaxHP)
{
	PlayerHPBar->SetPercent(HP, MaxHP);
}

void UPlayerUI::OnChangedPlayerStamina(float Stamina, float MaxStamina)
{
	PlayerStaminaBar->SetPercent(Stamina, MaxStamina);
}

void UPlayerUI::OnChangedBossHP(float HP, float MaxHP)
{
	BossHPBar->SetPercent(HP, MaxHP);
}
