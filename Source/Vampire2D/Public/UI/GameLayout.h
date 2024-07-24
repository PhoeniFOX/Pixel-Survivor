// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameLayout.generated.h"

class USkillTab;
struct FSkillInfo;
class UHorizontalBox;
class UImage;
class UOverlay;
class UVerticalBox;
class UButton;
class UTextBlock;
class UProgressBar;
class UWidgetSwitcher;
class USoundCue;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UGameLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	// Components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* WS_Main;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* WS_InGame;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* Bar_EXP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_Level;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_Time;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* Bar_HP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_CurHP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_MaxHP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_Kills;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_Lose;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_Win;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_FinalScore;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_CoinCollected;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Retry;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Return;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UVerticalBox* VB_SkillList;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UButton* BTN_Reroll;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_RerollCounts;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* HB_ActiveSkills;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* HB_PassiveSkills;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* O_Special;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UProgressBar* P_Special;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_SpecialReady;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* T2D_RandomBuffIcon;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_RandomBuffLv;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_RandomBuffTitle;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_RandomBuffDesc;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* T2D_RandomBuff;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UButton* BTN_RandomConfirm;

	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundCue* WinMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundCue* LoseMusic;

	// Functions

	UFUNCTION(BlueprintCallable)
	void UpdateHP(int32 InCurHP, int32 InMaxHP);

	UFUNCTION(BlueprintCallable)
	void UpdateEXPAndLevel(int32 InCurEXP, int32 InRequiredEXP, int32 InLevel);

	UFUNCTION(BlueprintCallable)
	void UpdateTime(int32 InSecond, int32 InMinute);

	UFUNCTION(BlueprintCallable)
	void UpdateKills(int32 InKills);

	void GameOver(bool InbIsWin);

	void UpdateGameOverUI(int32 InFinalScore, int32 InCoinCollected);

	void DisplaySkillIconOnSlot(const FSkillInfo& InSkillInfo);

	void UpdateSpecialTimer(float InSpecialValue);

	void InitRandomBuff(UObject* InObject, const FText& InLevel, const FText& InTitle, const FText& InDescription);

protected:
	UFUNCTION()
	void OnReleasedRerollBtn();

	UFUNCTION()
	void OnReleasedBtnRandomConfirm();

	UFUNCTION()
	void OnReleasedRetryBtn();

	UFUNCTION()
	void OnReleasedReturnBtn();
	
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillTab> SkillTabClass;
	
	TArray<USkillTab*> SkillTabs;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	int32 RerollCounts = 100; 
};
