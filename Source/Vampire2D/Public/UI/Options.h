// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Options.generated.h"

class UButton;
class USlider;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UOptions : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	USlider* S_BGM;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	USlider* S_SFX;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UButton* BTN_Close;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UButton* BTN_Menu;

protected:
	UFUNCTION()
	void OnReleasedCloseBtn();

	UFUNCTION()
	void OnReleasedMenuBtn();

	UFUNCTION()
	void OnBGMValueChanged(float Value);

	UFUNCTION()
	void OnSFXValueChanged(float Value);
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
};
