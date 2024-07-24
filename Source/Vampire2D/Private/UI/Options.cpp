// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Options.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"
#include "Data/Saves/SettingSave.h"
#include "Kismet/GameplayStatics.h"


void UOptions::NativeConstruct()
{
	Super::NativeConstruct();
	UGameplayStatics::SetGamePaused(this,true);
	BTN_Close->OnReleased.AddDynamic(this, &UOptions::OnReleasedCloseBtn);
	BTN_Menu->OnReleased.AddDynamic(this, &UOptions::OnReleasedMenuBtn);
	S_BGM->OnValueChanged.AddDynamic(this, &UOptions::OnBGMValueChanged);
	S_SFX->OnValueChanged.AddDynamic(this, &UOptions::OnSFXValueChanged);

	FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
	if (LevelName == "Level_Menu")
	{
		BTN_Menu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		BTN_Menu->SetVisibility(ESlateVisibility::Visible);
	}
	S_BGM->SetValue(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->SettingSaves->BGMVolumes);
	S_SFX->SetValue(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->SettingSaves->SFXVolumes);
}

void UOptions::NativeDestruct()
{
	Super::NativeDestruct();
	UGameplayStatics::SetGamePaused(this,false);
}

void UOptions::OnReleasedCloseBtn()
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	if (UGameplayStatics::SaveGameToSlot(GameInstance->SettingSaves, GameInstance->SettingSaveSlots, 0))
	{
		UGameplayStatics::SetSoundMixClassOverride(this,
			GameInstance->SoundMix,  
			GameInstance->BGMSoundClass,
			GameInstance->SettingSaves->BGMVolumes);

		UGameplayStatics::SetSoundMixClassOverride(this,
			GameInstance->SoundMix,
			GameInstance->SFXSoundClass,
			GameInstance->SettingSaves->SFXVolumes);
	}
	RemoveFromParent();
}

void UOptions::OnReleasedMenuBtn()
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	UGameplayStatics::SaveGameToSlot(GameInstance->SettingSaves, GameInstance->SettingSaveSlots, 0);
	UGameplayStatics::OpenLevel(this, FName("Level_Menu"));
}

void UOptions::OnBGMValueChanged(float Value)
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	GameInstance->SettingSaves->BGMVolumes = Value;
	UGameplayStatics::SetSoundMixClassOverride(this,
	GameInstance->SoundMix,  
	GameInstance->BGMSoundClass,
	Value);
}

void UOptions::OnSFXValueChanged(float Value)
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	GameInstance->SettingSaves->SFXVolumes = Value;
	UGameplayStatics::SetSoundMixClassOverride(this,
	GameInstance->SoundMix,
	GameInstance->SFXSoundClass,
	Value);
}
