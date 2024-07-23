// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyGameConfig.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "PlayerUI.generated.h"

class AGameUnit;
class AMyCharacter;
class UStatusProgressBar;
class USimpleDialogue;
class UCommonUserWidget;
/**
 * 
 */
UCLASS(BlueprintType)
class THIRDGAME_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void Dialogue(const FString& Text, float Duration = 2.0f);
	void OnOffAcceptUI(bool bValue);
	bool IsVisibleDialogue();
	void HideDialogue();
	void SetBossHUD(TObjectPtr<AGameUnit> Target);
	void ClearBossHUD(TObjectPtr<AGameUnit> Target);
	void OnOffLockOnHUD(bool bValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "WidgetAnim Func")
	void PlayDieHUDAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLockOnHUDLocation(AGameUnit* TargetUnit);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION(BlueprintCallable)
	void SetPlayerHPBarSize(float MaxHP);

	UFUNCTION(BlueprintCallable)
	void SetPlayerStaminaBarSize(float MaxStamina);

	UFUNCTION(BlueprintCallable)
	void OnChangedPlayerHP(float HP, float MaxHP);

	UFUNCTION(BlueprintCallable)
	void OnChangedPlayerStamina(float Stamina, float MaxStamina);

	UFUNCTION(BlueprintCallable)
	void OnChangedBossHP(float HP, float MaxHP);

private:
	float DefaultHPBarSize = 150;
	float DefaultStaminaSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusProgressBar> PlayerHPBar = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusProgressBar> PlayerStaminaBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusProgressBar> BossHPBar = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USimpleDialogue> SimpleDialogue = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCommonUserWidget> AcceptUI = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UImage> LockOnHUD = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AMyCharacter> Player = nullptr;

	FDelegateHandle CurrentBossHUDHandle;
};
