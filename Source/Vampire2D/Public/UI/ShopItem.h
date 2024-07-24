// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Vampire2D/Structs/FShopItemInfo.h"
#include "ShopItem.generated.h"

class UButton;
class UImage;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectedDelegate, UShopItem*, ShopItemRef, bool, Display);
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UShopItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Contour;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_ItemIcon;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv1;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv5;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv6;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv7;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* T_Lv8;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Selected;

	// Variables
	UPROPERTY()
	TArray<UImage*> LvIcons;

	UPROPERTY(BlueprintAssignable)
	FOnSelectedDelegate OnSelected;

	FShopItemInfo ShopSlotInfo;

	// Functions
	void RefundSelection();
	void InitSelection();
	void RefreshShopSkillIcon(FShopItemInfo InSlotInfo);
	void UpgradeShopItemIcon(int32 InSpecificLv);
	void OnSelect(bool InSelect);

protected:
	UFUNCTION()
	void OnBtnSelectedReleased();
	
	virtual void NativeConstruct() override;

private:

};
