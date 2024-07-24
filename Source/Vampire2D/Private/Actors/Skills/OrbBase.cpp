// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Skills/OrbBase.h"

#include "PaperFlipbookComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Vampire2D/Interfaces/CombatInterface.h"

// Sets default values
AOrbBase::AOrbBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AOrbBase::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AOrbBase::OnSphereOverlapBegin);
}

void AOrbBase::Initialize(APlayerCharacter* InDamageCauser, float InDamage, float InDamageMultiplier)
{
	DamageCauser = InDamageCauser;
	Damage = InDamage;
	DamageMultiplier = InDamageMultiplier;
}

// Called every frame
void AOrbBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrbBase::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

