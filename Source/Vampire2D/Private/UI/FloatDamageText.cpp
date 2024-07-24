// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FloatDamageText.h"

#include "Components/TextBlock.h"

void UFloatDamageText::UpdateFloatDamageText_Implementation(const FText& InDamage, FLinearColor InColor)
{
	DamageText->SetText(InDamage);
	DamageText->SetColorAndOpacity(InColor);
	if (FMath::RandBool())
	{
		StopAnimation(LeftFloatingAnim);
		StopAnimation(RightFloatingAnim);
		PlayAnimation(RightFloatingAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
	else
	{
		StopAnimation(LeftFloatingAnim);
		StopAnimation(RightFloatingAnim);
		PlayAnimation(LeftFloatingAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);	
	}
}
