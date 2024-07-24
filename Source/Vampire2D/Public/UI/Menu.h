// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

class UOptions;
struct FShopItemInfo;
class UUniformGridPanel;
class UShopItem;
class UImage;
class UTextBlock;
class USkillSelection;
class UButton;
class UCanvasPanel;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// Components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* WS;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* P_Main;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_StartGame;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Upgrade;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Leaderboard;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Quit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Options;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_DeleteSave;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* P_Upgrade;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USkillSelection* SS_SkillDetail;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Confirm;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Refund;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BTN_Return_U;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TXT_CoinNum;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* P_Leaderboard;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UUniformGridPanel* UGP;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_1;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_2;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_3;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_4;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_5;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_6;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_7;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_8;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_9;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_10;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_11;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_12;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_13;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_14;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UShopItem* ShopItem_15;

	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;

	bool IsShopItemUpgradable() const;

	void DisplayShopInfo(FShopItemInfo* InShopItemInfo) const;

protected:
	UFUNCTION()
	void OnBtnUpgradeReleased();
	
	UFUNCTION()
	void OnBtnQuitReleased();
	
	UFUNCTION()
	void OnBtnReturnUReleased();

	UFUNCTION()
	void OnBtnConfirmReleased();

	UFUNCTION()
	void OnBtnRefundReleased();

	UFUNCTION()
	void OnBtnStartGameReleased();

	UFUNCTION()
	void OnOptionBtnReleased();

	UFUNCTION()
	void OnLeaderboardBtnReleased();

	UFUNCTION()
	void OnDeleteSaveBtnReleased();
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SelectShopItem(UShopItem* ShopItemRef, bool bDisplay);
	
	void Initialization();

private:
	TArray<UShopItem*> UI_ShopItems;
	
	UPROPERTY()
	UShopItem* SelectedItemSlot;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UOptions> OptionsWidgetClass;

	bool RetrieveShopItemTag(const FString& InTag) const;
	void ResetCurrentSkill();
	void RefreshShopInfo();
	int32 GetRefund(FShopItemInfo* InShopItemInfo) const;
	void RefundAll();
};
