#pragma once
#include "PaperSprite.h"
#include "FShopItemInfo.generated.h"


class UImage;

USTRUCT(BlueprintType, Blueprintable)
struct FShopItemInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::FromString("ItemName");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentLv = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLv = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32,int32> NextCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UpgradeTag = "UpgradeTag";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description = FText::FromString("Description");
	
};
