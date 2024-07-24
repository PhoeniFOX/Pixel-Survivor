// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Patterns/GridPattern.h"

#include "Actors/LevelManager.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"

AGridPattern::AGridPattern()
{
	ChildActor4 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor4"));
	ChildActor5 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor5"));
	ChildActor6 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor6"));
	ChildActor7 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor7"));
	ChildActor8 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor8"));
	ChildActor9 = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActor9"));
	ChildActor4->SetupAttachment(RotationPlane);
	ChildActor5->SetupAttachment(RotationPlane);
	ChildActor6->SetupAttachment(RotationPlane);
	ChildActor7->SetupAttachment(RotationPlane);
	ChildActor8->SetupAttachment(RotationPlane);
	ChildActor9->SetupAttachment(RotationPlane);
}

void AGridPattern::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGridPattern::BeginPlay()
{
	Super::BeginPlay();
}

void AGridPattern::SpawnGrid()
{
	for (UChildActorComponent* ActorComponent : SpecifiedLocationRefs)
	{
		FVector SpawnLocation;
		if (AdjustSpawnLocation(ActorComponent->GetComponentLocation(), SpawnLocation))
		{
			UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->PoolingMobs(
				false,
				SpecifiedMobID,
				ActorComponent->GetComponentLocation(),
				false,
				ActorComponent->GetComponentLocation() + FVector(0,-1000, 0));
		}
	}

	for (UChildActorComponent* ActorComponent : AdditionalRefs)
	{
		FVector SpawnLocation;
		if (AdjustSpawnLocation(ActorComponent->GetComponentLocation(), SpawnLocation))
		{
			UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->PoolingMobs(
				false,
				SpecifiedMobID,
				ActorComponent->GetComponentLocation(),
				false,
				ActorComponent->GetComponentLocation() + FVector(-1000,0,0));
		}
	}

	if (SpawnIterator < MaxLoop)
	{
		SpawnIterator++;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(KeepSpawnTimerHandle);
		KeepSpawnTimerHandle.Invalidate();
	}
}

void AGridPattern::PreInitialize(int32 InMaxLoop)
{
	MaxLoop = InMaxLoop;
}

void AGridPattern::Initialize(int32 InSpecifiedID)
{
	Super::Initialize(InSpecifiedID);
	
	SpecifiedLocationRefs.Add(ChildActor);
	SpecifiedLocationRefs.Add(ChildActor1);
	SpecifiedLocationRefs.Add(ChildActor2);
	SpecifiedLocationRefs.Add(ChildActor3);
	SpecifiedLocationRefs.Add(ChildActor4);
	SpecifiedLocationRefs.Add(ChildActor5);
	
	AdditionalRefs.Add(ChildActor6);
	AdditionalRefs.Add(ChildActor7);
	AdditionalRefs.Add(ChildActor8);
	AdditionalRefs.Add(ChildActor9);
	
	SpawnIterator = 1;

	GetWorldTimerManager().SetTimer(KeepSpawnTimerHandle, this, &AGridPattern::SpawnGrid, 1.f, true);
}
