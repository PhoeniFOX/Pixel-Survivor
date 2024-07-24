// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"

#include "PaperFlipbookComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Data/VampireGameInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = SphereComponent;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetupAttachment(SphereComponent);

	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	PaperFlipbookComponent->SetupAttachment(SphereComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 100.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = true;
	ProjectileMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
}

void AProjectile::InitializeProperties(const FVector& OverallScale, float Damage, const FVector& ProjectileVelc,
	FName TarTag, AActor* InDamageCauser, UPaperFlipbook* NewFlipbook, const FVector& FlipbookScale,
	float LifeTime, const FRotator& FlipbookRotation)
{
	PaperFlipbookComponent->SetFlipbook(NewFlipbook);
	SphereComponent->SetWorldScale3D(OverallScale);
	PaperFlipbookComponent->SetWorldScale3D(FlipbookScale);
	ProjectileMovementComponent->Velocity = ProjectileVelc;
	ProjectileDamage = Damage;
	TargetTag = TarTag;
	DamageCauser = InDamageCauser;
	SetProjectileState(EProjectileState::EPS_InUse);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AProjectile::ResetStateByTimer, LifeTime, false);
	PaperFlipbookComponent->SetRelativeRotation(FlipbookRotation);
}

void AProjectile::ResetStateByTimer()
{
	PaperFlipbookComponent->SetFlipbook(nullptr);
	DamagedActors.Empty();
	ProjectileMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
	CapsuleComponent->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	SphereComponent->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	PaperFlipbookComponent->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetActorLocation(FVector(0, 1000, -10000));
	SetProjectileState(EProjectileState::EPS_InPool);
	PaperFlipbookComponent->SetRelativeRotation(FRotator(0,-180,-90));
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	if (OtherActor->ActorHasTag(TargetTag) && !DamagedActors.Contains(OtherActor))
	{
		DamagedActors.AddUnique(OtherActor);
		UVampireGameInstance* VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance());
		if (VampireGameInstance && VampireGameInstance->PlayerCharacter)
		{
			if (ICombatInterface::Execute_IGetIsDead(OtherActor))
			{
				return;
			}
			ICombatInterface::Execute_IHitHandler(OtherActor, DamageCauser, ProjectileDamage, 20);
		}
	}
	else
	{
	
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

