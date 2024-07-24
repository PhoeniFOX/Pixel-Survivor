// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Skills/HolyShrine.h"

#include "Character/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Vampire2D/Interfaces/CombatInterface.h"

// Sets default values
AHolyShrine::AHolyShrine()
{
	PrimaryActorTick.bCanEverTick = true;
	HSRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HolyShrineRoot"));
	RootComponent = HSRoot;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHolyShrine::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AHolyShrine::OnStaticMeshBeginOverlap);
	StaticMeshComponent->OnComponentEndOverlap.AddDynamic(this, &AHolyShrine::OnStaticMeshEndOverlap);
}

void AHolyShrine::AreaDamage()
{
	if (InAreaActors.Num() == 0) return;
	TArray<AActor*> TempActors = InAreaActors;
	for (AActor* InAreaActor : TempActors)
	{
		if (DamageCauser && !ICombatInterface::Execute_IGetIsDead(DamageCauser))
		{
			float Critical = UKismetMathLibrary::RandomBoolWithWeight(DamageCauser->GetCrtMul()) ? 1.5f : 1.f;
			
			float FinalDamage = Damage * Critical * DamageCauser->GetAtkMul() * DamageCauser->p_es_mul(InAreaActor) * DamageCauser->p_dc_mul();
			
			ICombatInterface::Execute_IHitHandler(InAreaActor, DamageCauser, FinalDamage, 0);
		}
	}
}

void AHolyShrine::Initialize(float InAttackInterval, float InDmg, float InDilation, APlayerCharacter* InDamageCauser)
{
	AttackInterval = InAttackInterval;
	Damage = InDmg;
	Dilation = InDilation;
	DamageCauser = InDamageCauser;
}

// Called every frame
void AHolyShrine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHolyShrine::OnStaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("enemy"))
	{
		if (!InAreaActors.Contains(OtherActor))
		{
			InAreaActors.AddUnique(OtherActor);
			// Slow down
			OtherActor->CustomTimeDilation = Dilation;
			if (!AreaDmgTimerHandle.IsValid())
			{
				GetWorldTimerManager().SetTimer(AreaDmgTimerHandle, this, &AHolyShrine::AreaDamage, AttackInterval, true);
			}
		}
	}
}

void AHolyShrine::OnStaticMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag("enemy"))
	{
		InAreaActors.Remove(OtherActor);
		OtherActor->CustomTimeDilation = 1.f;
	}
	if (InAreaActors.Num() == 0)
	{
		GetWorldTimerManager().ClearTimer(AreaDmgTimerHandle);
		AreaDmgTimerHandle.Invalidate();
	}
}

