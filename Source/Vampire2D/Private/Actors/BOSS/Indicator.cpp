// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BOSS/Indicator.h"

#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

AIndicator::AIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);
	LineSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("LineSprite"));
	LineSprite->SetupAttachment(RootComponent);
}

void AIndicator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

