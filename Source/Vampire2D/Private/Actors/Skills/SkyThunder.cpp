// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Skills/SkyThunder.h"

#include "NavigationSystem.h"
#include "Character/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Vampire2D/Interfaces/CombatInterface.h"

// Sets default values
ASkyThunder::ASkyThunder()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(BoxComponent);
	
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystemComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	FAttachmentTransformRules AttachmentTransformRules =  FAttachmentTransformRules::KeepRelativeTransform;
	ParticleSystemComponent->AttachToComponent(BoxComponent, AttachmentTransformRules);
}

void ASkyThunder::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkyThunder::OnBoxComponentBeginOverlap);
}

void ASkyThunder::Initialize(APlayerCharacter* InDamageCauser, float InDamage, float InDamageMultiplier, float InAttackInterval)
{
	Damage = InDamage;
	DamageCauser = InDamageCauser;
	DamageMultiplier = InDamageMultiplier;
	AttackInterval = InAttackInterval;
}

void ASkyThunder::RestartThunder()
{
	ParticleSystemComponent->Deactivate();
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(this);
	if (NavigationSystem)
	{
		FNavLocation ResultLocation;
		do
		{
			NavigationSystem->GetRandomPointInNavigableRadius(DamageCauser->GetActorLocation(), 800.f, ResultLocation);
		}
		while (!UGameInstanceFunctionLibrary::CheckWithinScreen(this, ResultLocation.Location));
		RandomLocation = ResultLocation.Location;
	}
	if (!SetActorLocation(RandomLocation))
	{
		FNavLocation ResultLocation;
		do
		{
			NavigationSystem->GetRandomPointInNavigableRadius(DamageCauser->GetActorLocation(), 800.f, ResultLocation);
		}
		while (!UGameInstanceFunctionLibrary::CheckWithinScreen(this, ResultLocation.Location));
		RandomLocation = ResultLocation.Location;
	}
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ParticleSystemComponent->Activate();
	// In rate is existing duration
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ASkyThunder::ResetThunder, .5);
}

void ASkyThunder::ClearAllTimer()
{
	GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	GetWorldTimerManager().ClearTimer(IntervalTimerHandle);
	DurationTimerHandle.Invalidate();
	IntervalTimerHandle.Invalidate();
}

void ASkyThunder::ResetThunder()
{
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetWorldTimerManager().SetTimer(IntervalTimerHandle, this, &ASkyThunder::RestartThunder, AttackInterval);
}

void ASkyThunder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ASkyThunder::OnBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("enemy"))
	{
		EnemyRef = OtherActor;
		if (DamageCauser && !ICombatInterface::Execute_IGetIsDead(DamageCauser))
		{
			float Critical = UKismetMathLibrary::RandomBoolWithWeight(DamageCauser->GetCrtMul()) ? 1.5f : 1.f;
			
			float FinalDamage = Damage * DamageMultiplier * Critical * DamageCauser->GetAtkMul() *
				DamageCauser->p_es_mul(OtherActor) * DamageCauser->p_dc_mul();
		ICombatInterface::Execute_IHitHandler(EnemyRef, DamageCauser, FinalDamage, 0);
		}
	}
}

