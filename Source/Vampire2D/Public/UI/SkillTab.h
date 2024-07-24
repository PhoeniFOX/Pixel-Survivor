// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTab.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API USkillTab : public UUserWidget
{
	GENERATED_BODY()

public:
	// Components
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* T2D_SkillIcon;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_SkillLevel;

	// Functions
	void UpdateIconAndLevel(UTexture2D* InIcon, int32 InLevel);
	
	FString GetSkillTag() const { return SkillTag; }
	void SetSkillTag(const FString& InSkillTag) { SkillTag = InSkillTag; }

private:

	// For comparison purpose
	FString SkillTag;
};
