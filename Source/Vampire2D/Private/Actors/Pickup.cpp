// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Pickup.h"

#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Data/VampireGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Vampire2D/Interfaces/InteractionInterface.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = SphereComponent;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetupAttachment(RootComponent);
	PaperSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PaperSprite"));
	PaperSprite->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance());
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlapBegin);
	
}

void APickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("player") && PickupState == EPickupState::EPS_Ready)
	{
		PlayerActor = OtherActor;
		PickupState = EPickupState::EPS_Moving;
		if (LoopingCount <= 4)
		{
			FOnTimelineEvent TimelineProgress;
			TimelineProgress.BindUFunction(this, FName("MagnetMovementFunction"));
			MagnetTimeline.SetTimelineLength(999.f);
			MagnetTimeline.AddEvent(0.f, TimelineProgress);
			MagnetTimeline.SetLooping(true);
			MagnetTimeline.PlayFromStart();
			LoopingCount++;
		}
		else
		{
			ReturnToPool();
		}
	}
}

void APickup::MagnetMovementFunction()
{
	if (PlayerActor)
	{
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), PlayerActor->GetActorLocation(), GetWorld()->DeltaTimeSeconds, 5.0f);
		NewLocation.Z = 17.f;
		SetActorLocation(NewLocation);
		if (GetDistanceTo(PlayerActor) <= 20.f)
		{
			ReturnToPool();
		}
	}
}

// Avoid Flicking (Method 1) 
void APickup::AvoidFlicking()
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += FMath::RandRange(1.f, 3.f);
	PaperSprite->SetWorldLocation(NewLocation);
}

void APickup::ReturnToPool()
{
	ItemEffect(PlayerActor);
	MagnetTimeline.Stop();
	MagnetTimeline = EmptyTimeline;
	PlayerActor = nullptr;
	SphereComponent->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SetActorHiddenInGame(true);
	SetActorLocation(FVector(1000, 0, -10000));
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickup::ResetLoopingCountAndCollision, 1.f, false);
}

void APickup::ResetLoopingCountAndCollision()
{
	LoopingCount = 0;
	SphereComponent->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Overlap);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetActorHiddenInGame(false);
	PickupState = EPickupState::EPS_InPool;
}

void APickup::ItemEffect(AActor* TargetActor) const
{
	if (!TargetActor) return;
	
	switch (PickupType)
	{
		case EPickupType::EPT_EXP1:
			VampireGameInstance->SoundEffect(ESoundType::EST_CoinSound);
			break;

		case EPickupType::EPT_EXP2:
			VampireGameInstance->SoundEffect(ESoundType::EST_CoinSound);
			break;

		case EPickupType::EPT_Magnet:
			UGameplayStatics::SpawnSoundAtLocation(this, MagnetSound , GetActorLocation());
			IInteractionInterface::Execute_ITriggerMagnet(TargetActor);
			break;
		
		default:
			break;
	}
	IInteractionInterface::Execute_IAddEXP(TargetActor, ExpValue);
}

void APickup::InitializePickup(EPickupType inPickupType, int32 inExpValue)
{
	PickupType = inPickupType;
	ExpValue = inExpValue;
	UPaperSprite* Sprite = nullptr;
	switch (PickupType)
	{
	case EPickupType::EPT_EXP1:
		Sprite = EXP01Sprite;
		break;

	case EPickupType::EPT_EXP2:
		Sprite = EXP02Sprite;
		break;
	case EPickupType::EPT_Magnet:
		Sprite = MagnetSprite;
		break;
		
		default:
			break;
	}
	PaperSprite->SetSprite(Sprite);
	AvoidFlicking();
	PickupState = EPickupState::EPS_Ready;
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MagnetTimeline.TickTimeline(DeltaTime);
}

