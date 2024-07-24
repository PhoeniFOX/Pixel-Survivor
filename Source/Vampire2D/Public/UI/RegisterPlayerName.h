// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "Blueprint/UserWidget.h"
// #include "RegisterPlayerName.generated.h"
//
// class UTextBlock;
// class UButton;
// class UEditableText;
// /**
//  * 
//  */
// UCLASS()
// class VAMPIRE2D_API URegisterPlayerName : public UUserWidget
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
// 	UEditableText* ET_Name;
//
// 	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
// 	UButton* BTN_ConfirmName;
//
// 	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
// 	UTextBlock* TXT_Hint;
// 	
// protected:
// 	UFUNCTION()
// 	void OnETNameTextChanged(const FText& Text);
//
// 	UFUNCTION()
// 	void OnBtnConfirmNameReleased();
// 	
// 	virtual void NativeConstruct() override;
//
// private:
// 	UFUNCTION()
// 	void OnBeginMenu(bool InSuccess);
//
// 	FText PlayerName;
// 	bool bIsConfirmCooling = false;
// };
