// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Skills/OrbContainer.h"

#include "Actors/Skills/OrbBase.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
AOrbContainer::AOrbContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	OrbRoot = CreateDefaultSubobject<USceneComponent>(TEXT("OrbRoot"));
	RootComponent = OrbRoot;
	OrbRotation = CreateDefaultSubobject<USceneComponent>(TEXT("OrbRotation"));
	OrbRotation->SetupAttachment(OrbRoot);
	
	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
}

// Called when the game starts or when spawned
void AOrbContainer::BeginPlay()
{
	Super::BeginPlay();
}

void AOrbContainer::SwordRotationOverTime() const
{
	OrbRotation->AddRelativeLocation(FVector(0.7, 0,0));
}

void AOrbContainer::CreateOrbs(float InRadius, int32 InOrbNum, float InOrbScale, AActor* InTarget,
                               float InAtkMultiplier, float InDamage, int32 InForce)
{
	if (!InTarget) return;
	SetActorLocation(InTarget->GetActorLocation());
	float UnitRadians = 2 * PI / InOrbNum;
	for (int32 i = 0; i < InOrbNum; i++)
	{
		float X = InRadius * FMath::Cos(UnitRadians * i) + InTarget->GetActorLocation().X;
		float Y = InRadius * FMath::Sin(UnitRadians * i) + InTarget->GetActorLocation().Y;
		FVector SpawnLocation = FVector(X, Y, InTarget->GetActorLocation().Z);
		AOrbBase* Orb = GetWorld()->SpawnActor<AOrbBase>(OrbClass, SpawnLocation, FRotator::ZeroRotator);
		Orb->SetActorScale3D(FVector(InOrbScale));
		Orb->Initialize(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter, InDamage, InAtkMultiplier);
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
		Orb->AttachToComponent(OrbRotation, AttachmentRules);
		OrbArray.Add(Orb);
	}
	StartRotation = OrbRoot->GetRelativeRotation();
}

void AOrbContainer::CreateSwords(float InRadius, int32 InOrbNum, float InOrbScale, AActor* InTarget,
	float InAtkMultiplier, float InDamage, int32 InForce)
{
	if (!InTarget) return;
	SetActorLocation(InTarget->GetActorLocation());
	float UnitRadians = 2 * PI / InOrbNum;
	for (int32 i = 0; i < InOrbNum; i++)
	{
		float X = InRadius * FMath::Cos(UnitRadians * i) + InTarget->GetActorLocation().X;
		float Y = InRadius * FMath::Sin(UnitRadians * i) + InTarget->GetActorLocation().Y;
		FVector SpawnLocation = FVector(X, Y, InTarget->GetActorLocation().Z);
		AOrbBase* Sword = GetWorld()->SpawnActor<AOrbBase>(SwordClass, SpawnLocation, FRotator::ZeroRotator);
		Sword->SetActorScale3D(FVector(InOrbScale));
		Sword->Initialize(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter, InDamage, InAtkMultiplier);
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
		Sword->AttachToComponent(OrbRotation, AttachmentRules);
		OrbArray.Add(Sword);
	}
	StartRotation = OrbRoot->GetRelativeRotation();

	FOnTimelineEvent TimelineEvent;
	TimelineEvent.BindUFunction(this, FName("SwordRotationOverTime"));
	SwordRotationTimeline.SetTimelineLength(999.f);
	SwordRotationTimeline.AddEvent(0.f, TimelineEvent);
	SwordRotationTimeline.SetLooping(true);
	SwordRotationTimeline.PlayFromStart();
}

void AOrbContainer::SetupRotationSpeed(float InSpeed)
{
	RotationSpeed = InSpeed;
	RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed * 180.f, 0.f);
}

// Called every frame
void AOrbContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SwordRotationTimeline.TickTimeline(DeltaTime);
}

