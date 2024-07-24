// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillSelection.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Vampire2D/Structs/FShopItemInfo.h"

void USkillSelection::NativeConstruct()
{
	Super::NativeConstruct();
	
	BTN_Confirm->OnHovered.AddDynamic(this, &USkillSelection::OnHoveredConfirmBtn);
	BTN_Confirm->OnUnhovered.AddDynamic(this, &USkillSelection::OnUnhoveredConfirmBtn);
	BTN_Confirm->OnReleased.AddDynamic(this, &USkillSelection::OnReleasedConfirmBtn);
}

void USkillSelection::UpdateSkillInfo()
{
	TXT_SkillName->SetText(FText::FromString(SkillInfo.SkillName.ToString()));
	TXT_SkillLevel->SetText(FText::Format(FText::FromString("Lv {0}"), SkillInfo.SkillLevel + 1));
	TXT_SkillDescription->SetText(FText::FromString(SkillInfo.SkillDescriptions[SkillInfo.SkillLevel].ToString()));
	T2D_SkillImage->SetBrushFromTexture(SkillInfo.SkillIcon);
}

void USkillSelection::UpdateInfoInShop(FShopItemInfo* InShopItemInfo)
{
	TXT_SkillName->SetText(InShopItemInfo->DisplayName);
	
	int32 CurLv = InShopItemInfo->CurrentLv + 1;
	FText SkillLv = CurLv > InShopItemInfo->MaxLv ? FText::FromString("MAX") : FText::Format(FTextFormat::FromString("Lv {0}"), CurLv); 
	TXT_SkillLevel->SetText(SkillLv);
	T2D_SkillImage->SetBrushResourceObject(InShopItemInfo->Icon);
	TXT_SkillDescription->SetText(InShopItemInfo->Description);
	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.UseGrouping = false;
	TXT_SkillCost->SetText(FText::Format(FTextFormat::FromString("Cost {0}"), FText::AsNumber(*InShopItemInfo->NextCost.Find(FMath::Clamp(InShopItemInfo->CurrentLv, 0, InShopItemInfo->MaxLv - 1)), &NumberFormattingOptions)));
	TXT_SkillCost->SetVisibility(*InShopItemInfo->NextCost.Find(FMath::Clamp(InShopItemInfo->CurrentLv, 0, InShopItemInfo->MaxLv - 1)) > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void USkillSelection::OnHoveredConfirmBtn()
{
	T_Selection->SetVisibility(ESlateVisibility::Visible);
}

void USkillSelection::OnUnhoveredConfirmBtn()
{
	T_Selection->SetVisibility(ESlateVisibility::Hidden);
}

void USkillSelection::OnReleasedConfirmBtn()
{
	OnSkillSelected.Broadcast(SkillInfo);
}

void USkillSelection::OnReleasedRerollBtn()
{
}


