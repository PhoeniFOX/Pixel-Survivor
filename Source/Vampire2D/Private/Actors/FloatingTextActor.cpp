// Fill out your copyright notice in the Description page of Project Settings.

// This actor is only spawned once when it is attacked by player character. Since Enemy is pooling, this actor is persistent until current level changes.

#include "Actors/FloatingTextActor.h"

#include "Components/WidgetComponent.h"
#include "UI/FloatDamageText.h"

AFloatingTextActor::AFloatingTextActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = SceneComponent;
	FloatingTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingTextWidget"));
	FloatingTextWidget->SetupAttachment(RootComponent);
}

void AFloatingTextActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFloatingTextActor::DisplayFloatingText(const FText& inText, const FLinearColor& inColor) const
{
	if (FloatingTextWidget)
	{
		UUserWidget* FloatingTextWidgetInstance = FloatingTextWidget->GetUserWidgetObject();
		if (UFloatDamageText* FloatingTextWidgetInstanceCasted = Cast<UFloatDamageText>(FloatingTextWidgetInstance))
		{
			FloatingTextWidgetInstanceCasted->UpdateFloatDamageText(inText, inColor);
		}
	}
}

// Called every frame
void AFloatingTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

