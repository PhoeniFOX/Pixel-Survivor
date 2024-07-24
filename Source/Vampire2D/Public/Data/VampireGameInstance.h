// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Vampire2D/Enums/SoundCueType.h"
#include "VampireGameInstance.generated.h"

class USoundCue;
struct FClientUpdateUserTitleDisplayNameResult;
struct FClientGetPlayerProfileResult;
struct FClientLoginResult;
struct FPlayFabError;
class USettingSave;
struct FShopItemInfo;
class UGameplaySave;
class APlayerCharacter;
class ALevelManager;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContinuousMenuDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginMenuDelegate, bool, bSuccess);

UCLASS()
class VAMPIRE2D_API UVampireGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual	void Init() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "References")
	ALevelManager* LevelManager;

	UPROPERTY(VisibleAnywhere, Category = "References")
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, Category = "References")
	APlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundMix* SoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundClass* SFXSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundClass* BGMSoundClass;

	TArray<FShopItemInfo> ShopItemInfos;
	
	int32 Coins = 0;

	UPROPERTY()
	UGameplaySave* GameplaySaves;

	UPROPERTY()
	USettingSave* SettingSaves;

	FString SaveSlots = "gameplaysav";
	FString SettingSaveSlots = "gamesettingsav";

	FString RandomEffectTag = "";

	bool bIsPlayFabDataLoad = false;
	bool bLeaderboardInit = false;

	FOnContinuousMenuDelegate OnContinuousMenu;
	FOnBeginMenuDelegate OnBeginMenu;
	
	// Functions
	void SoundEffect(ESoundType SoundType) const;
	void BGMManagement(USoundCue* InMusic);
	int32 AddCoins(int32 InAdd);
	FORCEINLINE int32 GetActiveSlotNum() const { return ActiveSlotNum; }
	void CheckSave();
	int32 RetrieveShopModifier(const FString& InTag);
	void GameSettingSave();
	
	// UFUNCTION()
	// void LoadDataFromPlayFab();
	//
	// UFUNCTION()
	// void PlayFabSuccess(FClientLoginResult result, UObject* customData);
	// UFUNCTION()
	// void PlayFabError(FPlayFabError ErrorResult, UObject* customData);
	// UFUNCTION()
	// void GetProfileSuccess(FClientGetPlayerProfileResult result, UObject* customData);
	// UFUNCTION()
	// void CreateTitleSuccess(FClientUpdateUserTitleDisplayNameResult result, UObject* customData);
	// UFUNCTION()
	// void SkipInputName(FPlayFabError ErrorResult, UObject* customData);
	// UFUNCTION()
	// void CreateAccountSuccess(FClientLoginResult result, UObject* customData);
	
	//
	// void RetrievePlayerDisplayName(const FString& InPlayFabID);
	// void CreatePlayFabAccountBasedOnName(const FText& InDesiredName);

protected:

private:
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundCue* HitSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundCue* CoinSoundCue;
	
	UPROPERTY()
	UAudioComponent* MusicAudioComponent;

	UPROPERTY(EditDefaultsOnly, Category="Save Game")
	UDataTable* ShopItemInfoDataTable;

	// used for leaderboard
	FString PlayerName = "";
	FString Token = "";		
	int32 HighScore = 0;
	FText DesiredName;
	

	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<UGameplaySave> GameplaySaveClass;

	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<USettingSave> SettingSaveClass;

	int32 ActiveSlotNum = 6;

};
