// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"

#include "NavigationSystem.h"
#include "PaperFlipbookComponent.h"
#include "Actors/FloatingTextActor.h"
#include "Actors/LevelManager.h"
#include "Actors/BOSS/Indicator.h"
#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "UI/GameLayout.h"
#include "Vampire2D/Structs/FMonsterInfo.h"

AEnemy::AEnemy()
{
	
	PrimaryActorTick.bCanEverTick = true;
	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	
}

void AEnemy::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("player"))
	{
		DamagedActor = OtherActor;
		TouchDamage();
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::TouchDamage, .5, true);
	}
}

void AEnemy::OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag("player") && !bIsDead)
	{
		DamagedActor = nullptr;
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		AttackTimerHandle.Invalidate();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance());

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnCapsuleOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnCapsuleOverlapEnd);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleDirection();
	if (bCanMoveToTouch)
	{
		bCanMoveToTouch = false;
		MoveToPlayer(bMovetoPlayer, DestinationLocation);
	}
	FadingOutTimeline.TickTimeline(DeltaTime);
}

void AEnemy::DisplayHealth(float inHealth) const
{
	
}

void AEnemy::TouchDamage()
{
	if (DamagedActor && DamagedActor->ActorHasTag("player") && !bIsDead)
	{
		ICombatInterface::Execute_IHitHandler(DamagedActor, this, InfoAttack, 0);
	}
}

void AEnemy::HandleDamageText(float InDamage)
{
	if(FloatingTextActorClass)
	{
		FVector SpawnLocation = PaperFlipbookComponent->GetComponentLocation();
		if (FloatingTextActor == nullptr)
		{
			FloatingTextActor = GetWorld()->SpawnActor<AFloatingTextActor>(FloatingTextActorClass, SpawnLocation, FRotator::ZeroRotator);
			FloatingTextActor->AttachToComponent(PaperFlipbookComponent, FAttachmentTransformRules::KeepWorldTransform);
		}
		FloatingTextActor->DisplayFloatingText(FText::FromString(FString::FromInt(FMath::TruncToInt(InDamage))), FLinearColor::White);
	}
}

void AEnemy::SpawnSetup(FName InMonsterID, bool bInbRandomLocation, const FVector& InTeleportLocation, bool bInbMovetoPlayer,
                        const FVector& InDestinationLocation)
{
	bRandomLocation = bInbRandomLocation;
	TeleportLocation = InTeleportLocation;
	bMovetoPlayer = bInbMovetoPlayer;
	DestinationLocation = InDestinationLocation;
	
	MonsterInfo = MonsterDataTable->FindRow<FMonsterInfo>(InMonsterID, TEXT(""), true);
	if (MonsterInfo)
	{
		bIsPooling = !MonsterInfo->bIsBoss;
		MonsterID = InMonsterID;
		InfoName = MonsterInfo->MonsterName;
		PaperFlipbookComponent->SetFlipbook(MonsterInfo->MonsterFlipbook);
		InfoHP = MonsterInfo->HP;
		InfoAttack = MonsterInfo->ATK;
		InfoInitWalkSpeed = MonsterInfo->SPD;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		GetCharacterMovement()->MaxWalkSpeed = InfoInitWalkSpeed;
		InfoMass = MonsterInfo->Mass;
		InfoExp = MonsterInfo->EXP;
		InfoTreasureDropRate = MonsterInfo->DropTreasureRate;
		GetWorldTimerManager().SetTimer(ReturnToPoolTimerHandle, this, &AEnemy::ReturnToPool, MonsterInfo->LifeTime, false);
		if (MonsterInfo->bIsBoss)
		{
			switch (MonsterInfo->ID)
			{
				case 16:
					GetWorldTimerManager().SetTimer(CheckDistanceTimerHandle, this, &AEnemy::Boss16, .1, true);
					break;
				
				case 31:
					GetWorldTimerManager().SetTimer(CheckDistanceTimerHandle, this, &AEnemy::Boss31, .1, true);
					break;
				
				default: break;
			}
		}
		if (TranslucentUnlitMaterial)
		{
			PaperFlipbookComponent->SetMaterial(0, TranslucentUnlitMaterial);
		}
		// if (MaskedUnlitMaterial)
		// {
		// 	
		// 	TransparentMaterial = PaperFlipbookComponent->CreateDynamicMaterialInstance(0, MaskedUnlitMaterial);
		// }
		StartMarching();
	}
}

void AEnemy::PoolingPickups()
{
	if (VampireGameInstance)
	{
		VampireGameInstance->LevelManager->PoolingPickupBase(EPickupType::EPT_EXP1, GetActorLocation(), InfoExp);
	}
}

void AEnemy::ForceDeath()
{
	PoolingDeath();
	ResetStatusByTimer();
}

void AEnemy::ReturnToPool()
{
	if (bool bIsDropMagnet = UKismetMathLibrary::RandomBoolWithWeight(0.05f))
	{
		if (VampireGameInstance)
		{
			VampireGameInstance->LevelManager->PoolingPickupBase(EPickupType::EPT_Magnet, GetActorLocation(), 0);
		}
	}
	else
	{
		PoolingPickups();
	}
	ForceDeath();
}

void AEnemy::StartMarching()
{
	if (bRandomLocation)
	{
		// Spawn enemy outside the screen
		GetWorldTimerManager().SetTimer(CheckSpawnLocationTimerHandle, this, &AEnemy::CheckSpawnLocation, 0.3f, true); 
	}
	else
	{
		TeleportTo(TeleportLocation, FRotator::ZeroRotator);
		PoolingRevive(bMovetoPlayer, DestinationLocation);
	}
}

void AEnemy::HandleDirection()
{
	if (bMovetoPlayer)
	{
		float PlayerX = UGameplayStatics::GetPlayerCharacter(GetWorld(),0)->GetActorLocation().X;
		float EnemyX = GetActorLocation().X;
		if (PlayerX > EnemyX && bToLeft)
		{
			bToLeft = false;
			PaperFlipbookComponent->SetRelativeRotation(FRotator(0, 0, -90));
		}
		else if (PlayerX < EnemyX && !bToLeft)
		{
			bToLeft = true;
			PaperFlipbookComponent->SetRelativeRotation(FRotator(180, 0, -90));
		}
	}
	else
	{
		if (bToLeft)
		{
			PaperFlipbookComponent->SetRelativeRotation(FRotator(180, 0, -90));
		}
		else
		{
			PaperFlipbookComponent->SetRelativeRotation(FRotator(0, 0, -90));
		}
	}

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::IHitHandler_Implementation(AActor* InDamageCauser, float InDamage, int32 InForce)
{
	if (InDamageCauser && !ICombatInterface::Execute_IGetIsDead(InDamageCauser))
	{
		CurrentHealth -= InDamage;
		DisplayHealth(CurrentHealth);
		HandleKnockBackAndColor(InDamageCauser, InForce);
		HandleDamageText(InDamage);
		if (VampireGameInstance)
		{
			VampireGameInstance->SoundEffect(ESoundType::EST_HitSound);
		}
		if (CurrentHealth <= 0)
		{
			ReturnToPool();
		}
	}
}

void AEnemy::ResetColorAndMovement()
{
	PaperFlipbookComponent->SetSpriteColor(FLinearColor::White);
	MoveToPlayer(bMovetoPlayer, DestinationLocation);
}

void AEnemy::HandleKnockBackAndColor(AActor* InDamageCauser, int32 InForce)
{
	if (!InDamageCauser || bIsDead) return;
	if (InForce > 0)
	{
		GetCharacterMovement()->StopMovementImmediately();
		float Force = InForce / InfoMass;
		FVector LaunchDirection = (GetActorLocation() - InDamageCauser->GetActorLocation()).GetSafeNormal();
		LaunchCharacter(LaunchDirection * Force, true, true);
	}
	PaperFlipbookComponent->SetSpriteColor(FLinearColor::Red);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::ResetColorAndMovement, 0.2f, false);
}

void AEnemy::MoveToPlayer(bool bInbMovetoPlayer, const FVector& InDestinationLocation)
{
	if (bInbMovetoPlayer)
	{
		RestartApproaching();
	}
	else
	{
		bToLeft = GetActorLocation().X >= InDestinationLocation.X;
		RestartApproachingLocation();
	}
}

void AEnemy::RestartApproaching()
{
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (AAIController* AC = Cast<AAIController>(GetController()))
		{
			AC->MoveToActor(Player);
		}
	}
}

void AEnemy::RestartApproachingLocation()
{
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (AAIController* AC = Cast<AAIController>(GetController()))
		{
			AC->MoveToLocation(DestinationLocation, 20.f);
		}
	}
}

void AEnemy::PoolingRevive(bool bInMoveToPlayer, const FVector& InDestinationLocation)
{
	bIsDead = false;
	CurrentHealth = InfoHP;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	// SetActorTickEnabled(bInMoveToPlayer);
	SetActorTickEnabled(true);
	MoveToPlayer(bInMoveToPlayer, InDestinationLocation);

	if (TranslucentUnlitMaterial)
	{
		PaperFlipbookComponent->SetMaterial(0, TranslucentUnlitMaterial);
	}
	TransparentMaterial = nullptr;
	MobState = EMobState::EMS_MoveToPlayer;

	// Actor may stick on the ground. So offset Z + 1;
	AddActorLocalOffset(FVector(0,0,1));
}

void AEnemy::CheckSpawnLocation()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FNavLocation ResultLocation;
		NavSys->GetRandomPointInNavigableRadius(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation(), 800, ResultLocation);
		if (!UGameInstanceFunctionLibrary::CheckWithinScreen(this, ResultLocation.Location))
		{
			ResultLocation.Location.Z += 10;
			if (TeleportTo(ResultLocation.Location, FRotator::ZeroRotator))
			{
				GetWorldTimerManager().ClearTimer(CheckSpawnLocationTimerHandle);
				CheckSpawnLocationTimerHandle.Invalidate();
				PoolingRevive(bMovetoPlayer, DestinationLocation);
			}
		}
	}
}

void AEnemy::PoolingDeath()
{
	// Reset status of the enemy
	DamagedActor = nullptr;
	bIsDead = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_WorldDynamic);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	AttackTimerHandle.Invalidate();
	GetWorldTimerManager().ClearTimer(ReturnToPoolTimerHandle);
	ReturnToPoolTimerHandle.Invalidate();
	GetWorldTimerManager().ClearTimer(RestartApproachingTimerHandle);
	RestartApproachingTimerHandle.Invalidate();
	
	GetWorldTimerManager().ClearTimer(RestartApproachLocTimerHandle);
	RestartApproachLocTimerHandle.Invalidate();
	bApproachLocTimerSet = false;
	

	// Trigger character kill count
	IInteractionInterface::Execute_ITriggerKill(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Recycle enemy after it exists in certain time.
void AEnemy::ResetStatusByTimer()
{
	if (FadingOutCurve)
	{

		TimelineProgress.BindUFunction(this, FName("FadingOutTimelineProgress"));
		FadingOutTimeline.AddInterpFloat(FadingOutCurve, TimelineProgress);
		FadingOutTimeline.SetLooping(false);
		
		TimelineFinished.BindUFunction(this, FName("AfterFadeOut"));
		FadingOutTimeline.SetTimelineFinishedFunc(TimelineFinished);

		FadingOutTimeline.PlayFromStart();
	}
}

void AEnemy::Boss16()
{
	if (bBoss16Cooling) return;
	bBoss16Cooling = true;
	Boss16ShootDirection = UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(),
			UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->GetActorLocation());
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	Indicator = GetWorld()->SpawnActor<AIndicator>(IndicatorClass,
		FVector(GetActorLocation().X, GetActorLocation().Y, 10),
		FRotator(0, Boss16ShootDirection.Yaw, 0),
		SpawnParameters);
	
	FTimerHandle ShootDelay;
	GetWorldTimerManager().SetTimer(ShootDelay, this, &AEnemy::Boss16DelayShoot, 1.f, false);
}

void AEnemy::Boss16DelayShoot()
{
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->PoolingProjectiles(
		GetActorLocation(),
		FVector(4,4,4),
		30,
		Boss16ShootDirection.Vector() * 500,
		FName("player"),
		this,
		BossBulletFlipBook,
		FVector(5,5,5),
		8
	);
	Indicator->Destroy();
	FTimerHandle ResetCoolingTimerHandle;
	GetWorldTimerManager().SetTimer(ResetCoolingTimerHandle, [&]{bBoss16Cooling = false;},3,false);
}

void AEnemy::Boss31()
{
	if (!bBoss16Cooling)
	{
		float YDifference = abs(GetActorLocation().Y - UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->GetActorLocation().Y);
		if (YDifference <= 55)
		{
			Boss31Indicator();
		}
	}
	if(!bBoss31Cooling)
	{
		bBoss31Cooling = true;
		if (UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->GetDistanceTo(this) > 200)
		{
			Boss31VFX = UGameplayStatics::SpawnEmitterAttached(
				Boss31Effect,
				GetCapsuleComponent(),
				NAME_None,
				FVector(0,0,-10),
				FRotator(90,0,0),
				FVector(.7)
				);
			FTimerHandle DelayTimerHandle;
			GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AEnemy::Boss31SkillStart, .5, false);
		}
	}
}

void AEnemy::Boss31SkillStart()
{
	GetCharacterMovement()->MaxWalkSpeed = InfoInitWalkSpeed * 2;
	PaperFlipbookComponent->SetFlipbook(Boss31SkillFlipBook);
	FTimerHandle DelayTimerHandle;
	GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AEnemy::Boss31SkillEnd, 2.5, false);
}

void AEnemy::Boss31SkillEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = InfoInitWalkSpeed;
	PaperFlipbookComponent->SetFlipbook(MonsterInfo->MonsterFlipbook);
	if (Boss31VFX)
	{
		Boss31VFX->DestroyComponent();
	}
	bBoss31Cooling = false;
}

void AEnemy::Boss31Indicator()
{
	if (bBoss16Cooling) return;
	bBoss16Cooling = true;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	Indicator = GetWorld()->SpawnActor<AIndicator>(IndicatorClass,
		FVector(GetActorLocation().X, GetActorLocation().Y, 10),
		FRotator::ZeroRotator,
		SpawnParameters);

	Indicator->SetActorScale3D(FVector(1));
	FTimerHandle ShootDelay;
	GetWorldTimerManager().SetTimer(ShootDelay, this, &AEnemy::Boss31DelayShoot, 1.f, false);
}

void AEnemy::Boss31DelayShoot()
{
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->PoolingProjectiles(
		GetActorLocation(),
		FVector(6),
		40,
		bToLeft ? FVector(-350,0,0) : FVector(350,0,0),
		FName("player"),
		this,
		BossBulletFlipBook,
		FVector(5.5),
		8
	);
	Indicator->Destroy();
	FTimerHandle ResetCoolingTimerHandle;
	GetWorldTimerManager().SetTimer(ResetCoolingTimerHandle, [&]{bBoss16Cooling = false;},3,false);
}

void AEnemy::FadingOutTimelineProgress(float Value)
{
	if (!TransparentMaterial)
	{
		if (MaskedUnlitMaterial)
		{
			TransparentMaterial = PaperFlipbookComponent->CreateDynamicMaterialInstance(0, MaskedUnlitMaterial);
		}
	}

	TransparentMaterial->SetScalarParameterValue(FName("Disappear"), Value);
}

void AEnemy::AfterFadeOut()
{
	SetActorTickEnabled(false);
	if (VampireGameInstance)
	{
		VampireGameInstance->LevelManager->RemoveEnemyFromActivePool(this);
		if (bIsPooling)
		{
			if (TeleportTo(FVector(0, 1000, -10000), FRotator::ZeroRotator))
			{
				MobState = EMobState::EMS_InPool;
				VampireGameInstance->LevelManager->TryRespawnMoreMobs();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to teleport enemy to pool"));
			}
		}
		else
		{
			// Not pooling then destroy the actor : usual for Boss
			if (MonsterID == "31" || MonsterID == "16")
			{
				if (MonsterID == "31")
				{
					UGameInstanceFunctionLibrary::GetGameLayout(this)->GameOver(true);
				}
				GetWorldTimerManager().ClearTimer(CheckDistanceTimerHandle);
				CheckDistanceTimerHandle.Invalidate();
			}
			else
			{
				//TBD
				if (Indicator)
				{
					Indicator->Destroy();
				}
				Destroy();
			}
		}
	}
}

void AEnemy::MoveToPlayerSuccess()
{
	GetWorldTimerManager().ClearTimer(RestartApproachingTimerHandle);
	RestartApproachingTimerHandle.Invalidate();
}

void AEnemy::MoveToPlayerFail()
{
	// Call this if MoveTo Failed
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	GetWorldTimerManager().SetTimer(RestartApproachingTimerHandle, this, &AEnemy::RestartApproaching, .2f, true);
}

void AEnemy::MoveToLocationSuccess()
{
	PoolingDeath();
	ResetStatusByTimer();
}

void AEnemy::MoveToLocationFail()
{
	if (bApproachLocTimerSet) return;
	GetWorldTimerManager().SetTimer(RestartApproachLocTimerHandle, this, &AEnemy::RestartApproachingLocation, .2f, true);
	bApproachLocTimerSet = true;
}

void AEnemy::ResetMoving()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&]()
		{
			bCanMoveToTouch = true;
		}, 1.f, false);
}

void AEnemy::SetMobState(EMobState InMobState)
{
	MobState = InMobState;
}

