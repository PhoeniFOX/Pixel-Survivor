// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTab.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void USkillTab::UpdateIconAndLevel(UTexture2D* InIcon, int32 InLevel)
{
	if (T2D_SkillIcon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("T2D_SkillIcon is not valid"));
		return;
	}else if (TXT_SkillLevel == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("TXT_SkillLevel is not valid"));
		return;
	}
	T2D_SkillIcon->SetBrushFromTexture(InIcon);
	UE_LOG(LogTemp, Warning, TEXT("UpdateIconAndLevel, InLevel = %d"), InLevel);
	TXT_SkillLevel->SetText(FText::Format(FText::FromString("Lv {0}"), InLevel + 1));
}
