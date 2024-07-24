// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Vampire2DPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/PlayerCharacter.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"
#include "UI/GameLayout.h"

AVampire2DPlayerController::AVampire2DPlayerController()
{
	bShowMouseCursor = true;
	VampireGameInstance = nullptr;
	GameLayout = nullptr;
}

void AVampire2DPlayerController::BeginPlay()
{
	Super::BeginPlay();
	VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance());
	if (VampireGameInstance)
	{
		VampireGameInstance->PlayerController = this;
	}
	//CreateLayout();
}

void AVampire2DPlayerController::CreateLayout()
{
	if (GameLayoutClass)
	{
		GameLayout = Cast<UGameLayout>(CreateWidget(GetWorld(), GameLayoutClass));
		if (GameLayout)
		{
			GameLayout->AddToViewport();
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(GameLayout->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}

UGameLayout* AVampire2DPlayerController::GetGameLayout()
{
	if(GameLayout == nullptr)
	{
		CreateLayout();
	}
	return GameLayout;
}
