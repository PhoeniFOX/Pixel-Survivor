// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/MenuPlayerController.h"

// #include "Data/GameInstanceFunctionLibrary.h"
// #include "Data/VampireGameInstance.h"
// #include "Data/Saves/GameplaySave.h"
// #include "Kismet/GameplayStatics.h"
#include "UI/Menu.h"
#include "UI/RegisterPlayerName.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetShowMouseCursor(true);
	CreateMenuUI();
	// UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	//
	// if (!GameInstance->bLeaderboardInit)
	// {
	// 	if (!UGameplayStatics::DoesSaveGameExist(GameInstance->SaveSlots,0))
	// 	{
	// 		URegisterPlayerName* RegisterWidget = CreateWidget<URegisterPlayerName>(this,RegisterPlayerNameWidgetClass);
	// 		RegisterWidget->AddToViewport();
	// 		
	// 		TSharedPtr<SWidget> SlateWidget = RegisterWidget->TakeWidget();
	// 		FInputModeGameAndUI InputModeGameAndUI;
	// 		InputModeGameAndUI.SetWidgetToFocus(SlateWidget);
	// 		InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 		InputModeGameAndUI.SetHideCursorDuringCapture(false);
	// 		this->SetInputMode(InputModeGameAndUI);
	// 	}
	// 	else
	// 	{
	// 		GameInstance->OnContinuousMenu.AddDynamic(this, &AMenuPlayerController::CreateMenuUI);
	// 		GameInstance->LoadDataFromPlayFab();
	// 	}
	// }
	// else
	// {
	// 	CreateMenuUI();
	// 	FTimerHandle TimerHandle;
	// 	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMenuPlayerController::DelayAfterCreateMenu, .2, false);
	// }
}

// void AMenuPlayerController::DelayAfterCreateMenu()
// {
// 	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
// 	GameInstance->CreatePlayFabAccountBasedOnName(FText::FromString(GameInstance->GameplaySaves->PlayerName));
// 	GameInstance->CheckSave();
// 	GameInstance->LoadDataFromPlayFab();
// }

void AMenuPlayerController::CreateMenuUI()
{
	UMenu* Menu = CreateWidget<UMenu>(this, MenuWidgetClass);
	Menu->AddToViewport();

	TSharedPtr<SWidget> SlateWidget = Menu->TakeWidget();
	FInputModeGameAndUI InputModeGameAndUI;
	InputModeGameAndUI.SetWidgetToFocus(SlateWidget);
	InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeGameAndUI.SetHideCursorDuringCapture(false);
	this->SetInputMode(InputModeGameAndUI);

}
