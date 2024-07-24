// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "UI/RegisterPlayerName.h"
//
// #include "Components/Button.h"
// #include "Components/EditableText.h"
// #include "Components/TextBlock.h"
// #include "Data/GameInstanceFunctionLibrary.h"
// #include "Data/MenuPlayerController.h"
// #include "Data/VampireGameInstance.h"
// #include "Kismet/GameplayStatics.h"
//
// void URegisterPlayerName::NativeConstruct()
// {
// 	Super::NativeConstruct();
// 	ET_Name->OnTextChanged.AddDynamic(this, &URegisterPlayerName::OnETNameTextChanged);
// 	BTN_ConfirmName->OnReleased.AddDynamic(this, &URegisterPlayerName::OnBtnConfirmNameReleased);
// 	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->OnBeginMenu.AddDynamic(this, &URegisterPlayerName::OnBeginMenu);
// 	TXT_Hint->SetText(FText::FromString("Input Your  Player  Name"));
// }
//
// void URegisterPlayerName::OnBeginMenu(bool InSuccess)
// {
// 	if (!InSuccess)
// 	{
// 		bIsConfirmCooling = false;
// 	}
// 	else
// 	{
// 		AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(UGameplayStatics::GetPlayerController(this,0));
// 		if (MenuPC)
// 		{
// 			MenuPC->CreateMenuUI();
// 			RemoveFromParent();
// 		}
// 	}
// }
//
// void URegisterPlayerName::OnETNameTextChanged(const FText& Text)
// {
// 	PlayerName = Text;
// }
//
// void URegisterPlayerName::OnBtnConfirmNameReleased()
// {
// 	if (!PlayerName.IsEmpty() && !PlayerName.IsNumeric() && !PlayerName.ToString().Contains(" "))
// 	{
// 		if (!bIsConfirmCooling)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("CreatePlayFabAccountBasedOnName(PlayerName);"));
// 			bIsConfirmCooling = true;
// 			UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->CreatePlayFabAccountBasedOnName(PlayerName);
// 		}
// 	}
// 	else
// 	{
// 		TXT_Hint->SetText(FText::FromString("Cannot contain Space or only numbers or space"));
// 	}
// }
