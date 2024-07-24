// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/VampireGameInstance.h"

// #include "PlayFabClientAPI.h"
// #include "PlayFabJsonValue.h"
#include "Components/AudioComponent.h"
#include "Data/Saves/GameplaySave.h"
#include "Data/Saves/SettingSave.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Vampire2D/Structs/FShopItemInfo.h"

void UVampireGameInstance::Init()
{
	Super::Init();

	// Initialize Sound
	FTimerHandle TimerHandle;
	GetTimerManager().SetTimer(TimerHandle, [&]()
	{
		UGameplayStatics::SetBaseSoundMix(this, SoundMix);
		GameSettingSave();
		UGameplayStatics::SetSoundMixClassOverride(this, SoundMix, SFXSoundClass, SettingSaves->SFXVolumes);
		UGameplayStatics::SetSoundMixClassOverride(this, SoundMix, BGMSoundClass, SettingSaves->BGMVolumes);
	}, .2, false);
}

void UVampireGameInstance::SoundEffect(ESoundType SoundType) const
{
	USoundCue* SoundCue = nullptr;
	switch (SoundType)
	{
		case ESoundType::EST_HitSound:
	 			SoundCue = HitSoundCue;
			break;
		case ESoundType::EST_CoinSound:
			SoundCue = CoinSoundCue;
			break;
		
		default: break;
	}
	if (SoundCue == nullptr) return;
	UGameplayStatics::PlaySound2D(this, SoundCue);
}

void UVampireGameInstance::BGMManagement(USoundCue* InMusic)
{
	if (MusicAudioComponent)
	{
		if (MusicAudioComponent->Sound != InMusic)
		{
			MusicAudioComponent->FadeOut(.1f, 0.f);
		}
		else
		{
			return;
		}
	}
	if (InMusic)
	{
		MusicAudioComponent = UGameplayStatics::SpawnSound2D(this, InMusic);
	}
}

int32 UVampireGameInstance::AddCoins(int32 InAdd)
{
	return Coins = FMath::Clamp(Coins + InAdd, 0, 999999);
}

void UVampireGameInstance::CheckSave()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlots,0))
	{
		GameplaySaves = Cast<UGameplaySave>(UGameplayStatics::LoadGameFromSlot(SaveSlots,0));
		if (GameplaySaves)
		{
			ShopItemInfos = GameplaySaves->ShopItemInfos;
			Coins = GameplaySaves->Coins;
			Token = GameplaySaves->Token;
			PlayerName = GameplaySaves->PlayerName;
		}
	}
	else
	{
		for (int i = 1; i < 50; i++)
		{
			FShopItemInfo* ShopItemInfo = ShopItemInfoDataTable->FindRow<FShopItemInfo>(FName(*FString::FromInt(i)), TEXT(""), false);
			if (ShopItemInfo)
			{
				ShopItemInfos.Add(*ShopItemInfo);
			}
		}
		GameplaySaves = Cast<UGameplaySave>(UGameplayStatics::CreateSaveGameObject(GameplaySaveClass));
		GameplaySaves->ShopItemInfos = ShopItemInfos;
		GameplaySaves->Coins = Coins;
		GameplaySaves->Token = FGuid::NewGuid().ToString();
		Token = GameplaySaves->Token;
		GameplaySaves->PlayerName = PlayerName;
		if (UGameplayStatics::SaveGameToSlot(GameplaySaves, SaveSlots, 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("Gameplay Save Success"));
			// send to leaderboard
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Gameplay Save Fail"));
		}
	}
}

int32 UVampireGameInstance::RetrieveShopModifier(const FString& InTag)
{
	for (FShopItemInfo& ItemInfo : ShopItemInfos)
	{
		if (ItemInfo.UpgradeTag == InTag)
		{
			return ItemInfo.CurrentLv;
		}
	}
	return -1;
}

void UVampireGameInstance::GameSettingSave()
{
	if (UGameplayStatics::DoesSaveGameExist(SettingSaveSlots,0))
	{
		SettingSaves = Cast<USettingSave>(UGameplayStatics::LoadGameFromSlot(SettingSaveSlots, 0));
	}
	else
	{
		SettingSaves = Cast<USettingSave>(UGameplayStatics::CreateSaveGameObject(SettingSaveClass));
		UGameplayStatics::SaveGameToSlot(SettingSaves, SettingSaveSlots, 0);
	}
}

// void UVampireGameInstance::LoadDataFromPlayFab()
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UVampireGameInstance::LoadDataFromPlayFab, bIsPlayFabDataLoad = %d"), bIsPlayFabDataLoad);
// 	if (bIsPlayFabDataLoad) return;
// 	bIsPlayFabDataLoad = true;
// 	CheckSave();
// 	FClientLoginWithCustomIDRequest LoginRequest;
// 	LoginRequest.CreateAccount = true;
// 	LoginRequest.CustomId = Token;
// 	UPlayFabClientAPI::FDelegateOnSuccessLoginWithCustomID onSuccess;
// 	onSuccess.BindUFunction(this, "PlayFabSuccess");
// 	UPlayFabClientAPI::FDelegateOnFailurePlayFabError onError;
// 	onError.BindUFunction(this, "PlayFabError");
// 	UPlayFabClientAPI::LoginWithCustomID(LoginRequest, onSuccess, onError, nullptr);
// 	FTimerHandle TimerHandle;
// 	GetTimerManager().SetTimer(TimerHandle, [&]{bIsPlayFabDataLoad = false;}, 1, false);
// }
//
// void UVampireGameInstance::PlayFabSuccess(FClientLoginResult result, UObject* customData)
// {
// 	RetrievePlayerDisplayName(result.PlayFabId);
// }
//
// void UVampireGameInstance::RetrievePlayerDisplayName(const FString& InPlayFabID)
// {
// 	FClientGetPlayerProfileRequest PlayerProfileRequest;
// 	PlayerProfileRequest.PlayFabId = InPlayFabID;
// 	UPlayFabClientAPI::FDelegateOnSuccessGetPlayerProfile onSuccess;
// 	onSuccess.BindUFunction(this, "GetProfileSuccess");
// 	UPlayFabClientAPI::FDelegateOnFailurePlayFabError onFailure;
// 	onFailure.BindUFunction(this, "PlayFabError");
// 	UPlayFabClientAPI::GetPlayerProfile(PlayerProfileRequest, onSuccess, onFailure, nullptr);
// }
//
// void UVampireGameInstance::CreatePlayFabAccountBasedOnName(const FText& InDesiredName)
// {
// 	DesiredName = InDesiredName;
// 	CheckSave();
// 	FClientLoginWithCustomIDRequest LoginRequest;
// 	LoginRequest.CreateAccount = true;
// 	LoginRequest.CustomId = Token;
// 	UPlayFabClientAPI::FDelegateOnSuccessLoginWithCustomID onSuccess;
// 	onSuccess.BindUFunction(this, "CreateAccountSuccess");
// 	UPlayFabClientAPI::FDelegateOnFailurePlayFabError onError;
// 	onError.BindUFunction(this, "PlayFabError");
// 	UE_LOG(LogTemp, Warning, TEXT("LoginWithCustomID"));
// 	UPlayFabClientAPI::LoginWithCustomID(LoginRequest, onSuccess, onError, nullptr);
// }
//
// void UVampireGameInstance::CreateAccountSuccess(FClientLoginResult result, UObject* customData)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UVampireGameInstance::CreateAccountSuccess"));
//
// 	FClientUpdateUserTitleDisplayNameRequest UpdateUserTitleDisplayNameRequest;
// 	UpdateUserTitleDisplayNameRequest.DisplayName = DesiredName.ToString();
// 	UPlayFabClientAPI::FDelegateOnSuccessUpdateUserTitleDisplayName onSuccess;
// 	onSuccess.BindUFunction(this, "CreateTitleSuccess");
// 	UPlayFabClientAPI::FDelegateOnFailurePlayFabError onSkip;
// 	onSkip.BindUFunction(this, "SkipInputName");
// 	UPlayFabClientAPI::UpdateUserTitleDisplayName(UpdateUserTitleDisplayNameRequest, onSuccess, onSkip, nullptr);
// }
//
// void UVampireGameInstance::PlayFabError(FPlayFabError error, UObject* customData)
// {
// 	UE_LOG(LogTemp, Error, TEXT("%s"), *error.ErrorMessage);
// }
//
// void UVampireGameInstance::SkipInputName(FPlayFabError ErrorResult, UObject* customData)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UVampireGameInstance::SkipInputName"));
//
// 	OnBeginMenu.Broadcast(false);
// }
//
//
// void UVampireGameInstance::GetProfileSuccess(FClientGetPlayerProfileResult result, UObject* customData)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UVampireGameInstance::GetProfileSuccess"));
//
// 	PlayerName = result.PlayerProfile->GetField("DisplayName")->AsString();
// 	bLeaderboardInit = true;
// 	OnContinuousMenu.Broadcast();
// }
//
// void UVampireGameInstance::CreateTitleSuccess(FClientUpdateUserTitleDisplayNameResult result, UObject* customData)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UVampireGameInstance::CreateTitleSuccess"));
// 	PlayerName = result.DisplayName;
// 	CheckSave();
// 	bLeaderboardInit = true;
// 	OnBeginMenu.Broadcast(true);
// }