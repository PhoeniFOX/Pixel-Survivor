// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameLayout.h"

#include "Actors/LevelManager.h"
#include "Character/PlayerCharacter.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SkillTab.h"
#include "Vampire2D/Structs/FSkillInfo.h"

#define LOCTEXT_NAMESPACE "CharacterLOCTEXT"

void UGameLayout::NativeConstruct()
{
	Super::NativeConstruct();
	BTN_Reroll->OnReleased.AddDynamic(this, &UGameLayout::OnReleasedRerollBtn);
	BTN_RandomConfirm->OnReleased.AddDynamic(this, &UGameLayout::OnReleasedBtnRandomConfirm);
	BTN_Retry->OnReleased.AddDynamic(this, &UGameLayout::OnReleasedRetryBtn);
	BTN_Return->OnReleased.AddDynamic(this, &UGameLayout::OnReleasedReturnBtn);
	RerollCounts = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->RetrieveShopModifier("rss_reroll");
	TXT_RerollCounts->SetText(FText::FromString(FString::FromInt(RerollCounts)));
}

void UGameLayout::UpdateHP(int32 InCurHP, int32 InMaxHP)
{
	float InCurHPFloat = static_cast<float>(InCurHP);
	float InMaxHPFloat = static_cast<float>(InMaxHP);
	Bar_HP->SetPercent(InCurHPFloat / InMaxHPFloat);
	TXT_CurHP->SetText(FText::FromString(FString::FromInt(InCurHP)));
	TXT_MaxHP->SetText(FText::FromString(FString::FromInt(InMaxHP)));
}

void UGameLayout::UpdateEXPAndLevel(int32 InCurEXP, int32 InRequiredEXP, int32 InLevel)
{
	float InCurEXPFloat = static_cast<float>(InCurEXP);
	float InRequiredEXPFloat = static_cast<float>(InRequiredEXP);
	Bar_EXP->SetPercent(InCurEXPFloat / InRequiredEXPFloat);
	TArray<FFormatArgumentValue> Args;
	Args.Add(InLevel);
	FText LevelText = FText::Format(LOCTEXT("LevelTEXT", "Lv {0}"), Args);
	TXT_Level->SetText(LevelText);
}

void UGameLayout::UpdateTime(int32 InMinute, int32 InSecond)
{
	TXT_Time->SetText(FText::FromString(FString::Printf(TEXT("%02d   %02d"), InMinute, InSecond)));
}

void UGameLayout::UpdateKills(int32 InKills)
{
	TXT_Kills->SetText(FText::FromString(FString::FromInt(InKills)));
}

void UGameLayout::GameOver(bool InbIsWin)
{
	UGameplayStatics::SetGamePaused(this, true);
	// Calculate Score
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->CalculateFinalScore();

	WS_Main->SetActiveWidgetIndex(1);
	if (InbIsWin)
	{
		TXT_Win->SetVisibility(ESlateVisibility::Visible);
		TXT_Lose->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TXT_Lose->SetVisibility(ESlateVisibility::Visible); 
		TXT_Win->SetVisibility(ESlateVisibility::Collapsed);
	}

	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->BGMManagement(InbIsWin ? WinMusic : LoseMusic);
}

void UGameLayout::UpdateGameOverUI(int32 InFinalScore, int32 InCoinCollected)
{
	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.UseGrouping = false;
	FText InFinalScoreText = FText::AsNumber(InFinalScore, &NumberFormattingOptions);
	FText InCoinCollectedText = FText::AsNumber(InCoinCollected, &NumberFormattingOptions);
	
	FText FinalScoreText = FText::Format(LOCTEXT("FinalScoreTEXT", "Score  {0}"), InFinalScoreText);
	TXT_FinalScore->SetText(FinalScoreText);
	
	FText CoinCollectedText = FText::Format(LOCTEXT("CoinCollectedTEXT", "Coins  {0}"), InCoinCollectedText);
	TXT_CoinCollected->SetText(CoinCollectedText);
}

void UGameLayout::DisplaySkillIconOnSlot(const FSkillInfo& InSkillInfo)
{
	USkillTab* TargetSkillTab = nullptr;
	for (USkillTab* SkillTab : SkillTabs)
	{
		if (SkillTab->GetSkillTag() == InSkillInfo.SkillTag)
		{
			TargetSkillTab = SkillTab;
			break;
		}
	}
	if (TargetSkillTab)
	{
		TargetSkillTab->UpdateIconAndLevel(InSkillInfo.SkillIcon, InSkillInfo.SkillLevel);
	}
	else
	{
		TargetSkillTab = CreateWidget<USkillTab>(this, SkillTabClass);
		TargetSkillTab->SetSkillTag(InSkillInfo.SkillTag);
		if (InSkillInfo.bIsItem)
		{
			HB_PassiveSkills->AddChild(TargetSkillTab);
		}
		else
		{
			HB_ActiveSkills->AddChild(TargetSkillTab);
		}
		SkillTabs.AddUnique(TargetSkillTab);
		
		TargetSkillTab->SetPadding(FMargin(2.f, 0.f, 0.f, 0.f));
		TargetSkillTab->UpdateIconAndLevel(InSkillInfo.SkillIcon, InSkillInfo.SkillLevel);
	}
}

void UGameLayout::UpdateSpecialTimer(float InSpecialValue)
{
	float SpecialPercent = FMath::Clamp(InSpecialValue, 0.f, 10.f) / 10.f;
	P_Special->SetPercent(SpecialPercent);
	TXT_SpecialReady->SetVisibility(InSpecialValue >= 10.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UGameLayout::InitRandomBuff(UObject* InObject, const FText& InLevel, const FText& InTitle,
	const FText& InDescription)
{
	UGameplayStatics::SetGamePaused(this,true);
	T2D_RandomBuff->SetBrushResourceObject(InObject);
	T2D_RandomBuffIcon->SetBrushResourceObject(InObject);
	TXT_RandomBuffTitle->SetText(InTitle);
	TXT_RandomBuffLv->SetText(InLevel);
	TXT_RandomBuffDesc->SetText(InDescription);
}

void UGameLayout::OnReleasedRerollBtn()
{
	if (RerollCounts == 0) return;
	BTN_Reroll->SetIsEnabled(--RerollCounts != 0);
	TXT_RerollCounts->SetText(FText::FromString(FString::FromInt(RerollCounts)));
	// Refresh 4 skills from skill pools again
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->AddSkillSelection();
}

void UGameLayout::OnReleasedBtnRandomConfirm()
{
	UGameplayStatics::SetGamePaused(this, false);
	WS_InGame->SetActiveWidgetIndex(0);
	T2D_RandomBuffIcon->SetVisibility(ESlateVisibility::Visible);
	TXT_RandomBuffLv->SetVisibility(ESlateVisibility::Visible);
}

void UGameLayout::OnReleasedRetryBtn()
{
	UGameplayStatics::OpenLevel(this, FName("Level_01"));
}

void UGameLayout::OnReleasedReturnBtn()
{
	UGameplayStatics::OpenLevel(this, FName("Level_Menu"));
}

#undef LOCTEXT_NAMESPACE
