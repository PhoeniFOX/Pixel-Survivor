// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItem.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Vampire2D/Structs/FShopItemInfo.h"

void UShopItem::RefundSelection()
{
	OnSelected.Broadcast(this, false);
}

void UShopItem::InitSelection()
{
	OnSelected.Broadcast(this, true);
}

void UShopItem::RefreshShopSkillIcon(FShopItemInfo InSlotInfo)
{
	ShopSlotInfo = InSlotInfo;
	T_ItemIcon->SetBrushResourceObject(ShopSlotInfo.Icon);
	for (int i = 0; i < ShopSlotInfo.MaxLv; i++)
	{
		LvIcons[i]->SetVisibility(ESlateVisibility::Visible);
	}
	UpgradeShopItemIcon(ShopSlotInfo.CurrentLv - 1);
}

void UShopItem::UpgradeShopItemIcon(int32 InSpecificLv)
{
	int i = 0;
	for (UImage* LvIcon : LvIcons)
	{
		if (i < InSpecificLv)
		{
			LvIcon->SetColorAndOpacity(FLinearColor::Blue);
		}
		else
		{
			LvIcon->SetColorAndOpacity(FLinearColor::Gray);
		}
		i++;
	}
}

void UShopItem::OnSelect(bool InSelect)
{
	T_Contour->SetVisibility(InSelect ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UShopItem::OnBtnSelectedReleased()
{
	OnSelected.Broadcast(this, true);
}

void UShopItem::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_Selected->OnReleased.AddDynamic(this, &UShopItem::OnBtnSelectedReleased);
	
	LvIcons.Add(T_Lv1);
	LvIcons.Add(T_Lv2);
	LvIcons.Add(T_Lv3);
	LvIcons.Add(T_Lv4);
	LvIcons.Add(T_Lv5);
	LvIcons.Add(T_Lv6);
	LvIcons.Add(T_Lv7);
	LvIcons.Add(T_Lv8);
}
