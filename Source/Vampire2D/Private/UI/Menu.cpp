// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"
#include "Data/Saves/GameplaySave.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Options.h"
#include "UI/ShopItem.h"
#include "UI/SkillSelection.h"
#include "Vampire2D/Structs/FShopItemInfo.h"

void UMenu::NativeConstruct()
{
	Super::NativeConstruct();
	BTN_Upgrade->OnReleased.AddDynamic(this, &UMenu::OnBtnUpgradeReleased);
	BTN_Quit->OnReleased.AddDynamic(this, &UMenu::OnBtnQuitReleased);
	BTN_Return_U->OnReleased.AddDynamic(this, &UMenu::OnBtnReturnUReleased);
	BTN_Confirm->OnReleased.AddDynamic(this, &UMenu::OnBtnConfirmReleased);
	BTN_Refund->OnReleased.AddDynamic(this, &UMenu::OnBtnRefundReleased);
	BTN_StartGame->OnReleased.AddDynamic(this, &UMenu::OnBtnStartGameReleased);
	BTN_Options->OnReleased.AddDynamic(this, &UMenu::OnOptionBtnReleased);
	BTN_Leaderboard->OnReleased.AddDynamic(this, &UMenu::OnLeaderboardBtnReleased);
	BTN_DeleteSave->OnReleased.AddDynamic(this, &UMenu::OnDeleteSaveBtnReleased);

	WS->SetActiveWidgetIndex(0);
	Initialization();
}

void UMenu::SelectShopItem(UShopItem* ShopItemRef, bool bDisplay)
{
	SelectedItemSlot = ShopItemRef;
	if (bDisplay)
	{
		for (UShopItem* Item : UI_ShopItems)
		{
			Item->OnSelect(false);
		}
		SelectedItemSlot->OnSelect(true);
	}
	SS_SkillDetail->UpdateInfoInShop(&SelectedItemSlot->ShopSlotInfo);
}

void UMenu::Initialization()
{
	BTN_Leaderboard->SetIsEnabled(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->bLeaderboardInit);
	PlayAnimationForward(FadeInAnim);
	for (UWidget* LShopItem : UGP->GetAllChildren())
	{
		if (UShopItem* Item = Cast<UShopItem>(LShopItem))
		{
			UI_ShopItems.AddUnique(Item);
			Item->OnSelected.AddDynamic(this, &UMenu::SelectShopItem);
		}
	}
	// Check Save
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->CheckSave();
	int32 i = 0;
	for (UShopItem* LShopItem : UI_ShopItems)
	{
		LShopItem->RefreshShopSkillIcon(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->ShopItemInfos[i]);
		LShopItem->UpgradeShopItemIcon(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->ShopItemInfos[i].CurrentLv);
		i++;
	}
	ShopItem->InitSelection();
	TXT_CoinNum->SetText(FText::FromString(FString::FromInt(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins)));
}

bool UMenu::RetrieveShopItemTag(const FString& InTag) const
{
	return SelectedItemSlot->ShopSlotInfo.UpgradeTag == InTag;
}

void UMenu::ResetCurrentSkill()
{
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins += GetRefund(&SelectedItemSlot->ShopSlotInfo);
	SelectedItemSlot->ShopSlotInfo.CurrentLv = 0;
	DisplayShopInfo(&SelectedItemSlot->ShopSlotInfo);
	RefreshShopInfo();
}

void UMenu::RefreshShopInfo()
{
	UVampireGameInstance* Instance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	Instance->ShopItemInfos.Empty();
	for (UWidget* WItem : UGP->GetAllChildren())
	{
		if (UShopItem* Item = Cast<UShopItem>(WItem))
		{
			Instance->ShopItemInfos.Add(Item->ShopSlotInfo);
		}
	}
	
	Instance->GameplaySaves->ShopItemInfos = Instance->ShopItemInfos;
	Instance->GameplaySaves->Coins = Instance->Coins;
	if(!UGameplayStatics::SaveGameToSlot(Instance->GameplaySaves, Instance->SaveSlots, 0))
	{
		UE_LOG(LogTemp, Error,TEXT("Shop Info Save fail."));
	}
}

int32 UMenu::GetRefund(FShopItemInfo* InShopItemInfo) const
{
	if (InShopItemInfo->CurrentLv == 0) return 0;
	int32 RefundSum = 0;
	for (int32 i = 0; i < InShopItemInfo->CurrentLv; i++)
	{
		RefundSum += *InShopItemInfo->NextCost.Find(i);
	}
	return RefundSum;
}

void UMenu::RefundAll()
{
	for (UShopItem* Item : UI_ShopItems)
	{
		Item->RefundSelection();
		ResetCurrentSkill();
	}
}

bool UMenu::IsShopItemUpgradable() const
{
	int32 CurLv = SelectedItemSlot->ShopSlotInfo.CurrentLv;
	int32 MaxLv = SelectedItemSlot->ShopSlotInfo.MaxLv;
	int32 Cost = *SelectedItemSlot->ShopSlotInfo.NextCost.Find(FMath::Clamp(CurLv, 0, MaxLv - 1));
	if (UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins >= Cost && CurLv < MaxLv)
	{
		return true;
	}
	return false;
}

void UMenu::DisplayShopInfo(FShopItemInfo* InShopItemInfo) const
{
	int32 Coin = FMath::Clamp(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins, 0, 999999);
	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.UseGrouping = false;
	TXT_CoinNum->SetText(FText::AsNumber(Coin, &NumberFormattingOptions));
	SS_SkillDetail->UpdateInfoInShop(InShopItemInfo);
	SelectedItemSlot->UpgradeShopItemIcon(InShopItemInfo->CurrentLv);
}

void UMenu::OnBtnUpgradeReleased()
{
	WS->SetActiveWidgetIndex(1);
}

void UMenu::OnBtnQuitReleased()
{
	UKismetSystemLibrary::QuitGame(this, UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerController,EQuitPreference::Quit,false);
}

void UMenu::OnBtnReturnUReleased()
{
	WS->SetActiveWidgetIndex(0);
}

void UMenu::OnBtnConfirmReleased()
{
	if(!IsShopItemUpgradable()) return;
	if (RetrieveShopItemTag("rss_refund"))
	{
		RefundAll();
	}
	int32 Coin = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins;
	int32 Cost = *SelectedItemSlot->ShopSlotInfo.NextCost.Find(FMath::Clamp(SelectedItemSlot->ShopSlotInfo.CurrentLv, 0, SelectedItemSlot->ShopSlotInfo.MaxLv - 1));
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins = FMath::Clamp(Coin - Cost, 0,999999);
	SelectedItemSlot->ShopSlotInfo.CurrentLv++;
	DisplayShopInfo(&SelectedItemSlot->ShopSlotInfo);
	if (RetrieveShopItemTag("rss_refund"))
	{
		ResetCurrentSkill();
	}
	else
	{
		RefreshShopInfo();
	}
}

void UMenu::OnBtnRefundReleased()
{
	// Refund current Skill
	ResetCurrentSkill();
}

void UMenu::OnBtnStartGameReleased()
{
	UGameplayStatics::OpenLevel(this, FName("Level_01"));
}

void UMenu::OnOptionBtnReleased()
{
	UOptions* Widget = CreateWidget<UOptions>(this, OptionsWidgetClass);
	if (Widget)
	{
		Widget->AddToViewport();
	}
}

void UMenu::OnLeaderboardBtnReleased()
{
	
}

void UMenu::OnDeleteSaveBtnReleased()
{
	if (UGameplayStatics::DeleteGameInSlot(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->SaveSlots,0))
	{
		UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->ShopItemInfos.Empty();
		UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->Coins = 0;
		UGameplayStatics::OpenLevel(this, FName("Level_Menu"));
	}
}

