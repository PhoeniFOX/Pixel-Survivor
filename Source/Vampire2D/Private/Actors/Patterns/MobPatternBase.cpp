// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Patterns/MobPatternBase.h"

#include "NavigationSystem.h"

AMobPatternBase::AMobPatternBase()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<UChildActorComponent>(TEXT("Root"));
	SetRootComponent(Root);
	RotationPlane = CreateDefaultSubobject<UChildActorComponent>(TEXT("RotationPlane"));
	RotationPlane->SetupAttachment(Root);
	ChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor"));
	ChildActor1 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor1"));
	ChildActor2 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor2"));
	ChildActor3 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor3"));
	ChildActor->SetupAttachment(RotationPlane);
	ChildActor1->SetupAttachment(RotationPlane);
	ChildActor2->SetupAttachment(RotationPlane);
	ChildActor3->SetupAttachment(RotationPlane);
}

void AMobPatternBase::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AMobPatternBase::AdjustSpawnLocation(const FVector& InOrigin, FVector& OutRandomLocation)
{
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(this);
	FNavLocation ResultLocation;
	if (NavigationSystem->GetRandomPointInNavigableRadius(InOrigin, 50.f, ResultLocation))
	{
		OutRandomLocation = ResultLocation.Location;
		return true;
	}
	return false;
}

void AMobPatternBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMobPatternBase::Initialize(int32 InSpecifiedID)
{
	SpecifiedMobID = InSpecifiedID;
}

