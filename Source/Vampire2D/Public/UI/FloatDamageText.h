// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatDamageText.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UFloatDamageText : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateFloatDamageText(const FText& InDamage, FLinearColor InColor);

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* DamageText;

	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* RightFloatingAnim;

	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* LeftFloatingAnim;
	
};
