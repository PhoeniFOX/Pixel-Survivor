// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplaySave.generated.h"

struct FShopItemInfo;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UGameplaySave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FShopItemInfo> ShopItemInfos;

	UPROPERTY(EditDefaultsOnly)
	int32 Coins;

	UPROPERTY(EditDefaultsOnly)
	FString Token;

	UPROPERTY(EditDefaultsOnly)
	FString PlayerName;
};
