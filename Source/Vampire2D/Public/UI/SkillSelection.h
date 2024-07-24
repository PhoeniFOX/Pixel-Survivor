// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Vampire2D/Structs/FSkillInfo.h"
#include "SkillSelection.generated.h"

struct FShopItemInfo;
class UVerticalBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillSelectedDelegate, const FSkillInfo&, SkillInfo);

class UButton;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API USkillSelection : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* T_Selection;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* T2D_SkillImage;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_SkillName;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_SkillCost;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_SkillLevel;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* TXT_SkillDescription;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UButton* BTN_Confirm;
	
	UPROPERTY(BlueprintAssignable)
	FOnSkillSelectedDelegate OnSkillSelected;

	void SetSkillInfo(const FSkillInfo& InSkillInfo) { SkillInfo = InSkillInfo; UpdateSkillInfo(); }

	void UpdateInfoInShop(FShopItemInfo* InShopItemInfo);

protected:
	UFUNCTION()
	void OnHoveredConfirmBtn();
	
	UFUNCTION()
	void OnUnhoveredConfirmBtn();
	
	UFUNCTION()
	void OnReleasedConfirmBtn();

	UFUNCTION()
	void OnReleasedRerollBtn();
	
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	FSkillInfo SkillInfo;

	void UpdateSkillInfo();
};
