// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Patterns/CirclePattern.h"

ACirclePattern::ACirclePattern()
{
	ChildActor4 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor4"));
	ChildActor5 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor5"));
	ChildActor6 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor6"));
	ChildActor7 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor7"));
	ChildActor4->SetupAttachment(RotationPlane);
	ChildActor5->SetupAttachment(RotationPlane);
	ChildActor6->SetupAttachment(RotationPlane);
	ChildActor7->SetupAttachment(RotationPlane);
}

void ACirclePattern::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ACirclePattern::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACirclePattern::Initialize(int32 InSpecifiedID)
{
	SpecifiedLocationRefs.Add(ChildActor4);
	SpecifiedLocationRefs.Add(ChildActor5);
	SpecifiedLocationRefs.Add(ChildActor6);
	SpecifiedLocationRefs.Add(ChildActor7);
	
	Super::Initialize(InSpecifiedID);
}
