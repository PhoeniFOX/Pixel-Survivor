// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LevelManager.h"

#include "Actors/Pickup.h"
#include "Actors/Projectile.h"
#include "Actors/Patterns/GridPattern.h"
#include "Actors/Patterns/MobPatternBase.h"
#include "Character/Enemy.h"
#include "Character/PlayerCharacter.h"
#include "Components/WidgetSwitcher.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/Vampire2DPlayerController.h"
#include "Data/VampireGameInstance.h"
#include "Data/Saves/GameplaySave.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"
#include "UI/GameLayout.h"
#include "Vampire2D/Enums/PatternTypes.h"
#include "Vampire2D/Enums/ProjectileState.h"
#include "Vampire2D/Structs/FShopItemInfo.h"

// Sets default values
ALevelManager::ALevelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (UVampireGameInstance* VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance()))
	{
		VampireGameInstance->LevelManager = this;
	}
	GetWorldTimerManager().SetTimer(TimeCountTimerHandle, this, &ALevelManager::TimeCount, 1.f, true);
}

// Called every frame
void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsRandomEventConfirmed && UGameInstanceFunctionLibrary::GetGameLayout(this))
	{
		RandomEvent();
		bIsRandomEventConfirmed = true;
	}
}

void ALevelManager::PoolingProjectiles(const FVector& Location, const FVector& OverallScale, float Damage,
                                       const FVector& ProjectileVelocity, FName TargetTag, AActor* DamageCauser, UPaperFlipbook* NewFlipbook,
                                       const FVector& FlipbookScale, float LifeTime, const FRotator& InFlipbookRotation)
{
	AProjectile* AvailableProjectile = nullptr;
	for (AProjectile* Projectile : ProjectilePool)
	{
		if (Projectile->GetProjectileState() == EProjectileState::EPS_InPool)
		{
			AvailableProjectile = Projectile;
			break;
		}
	}
	if (!AvailableProjectile)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AvailableProjectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			FVector(0, 1000, -10000),
			FRotator::ZeroRotator,
			SpawnParameters
			);
		ProjectilePool.Add(AvailableProjectile);
	}
	AvailableProjectile->SetActorLocation(Location);
	AvailableProjectile->InitializeProperties(OverallScale, Damage, ProjectileVelocity, TargetTag, DamageCauser, NewFlipbook, FlipbookScale, LifeTime, InFlipbookRotation);
}

void ALevelManager::PoolingMobs(bool bInIsRandom, int32 InID, const FVector& InLocation, bool bInMoveToPlayer,
	const FVector& InDestination)
{
	AEnemy* AvailableEnemy = nullptr;
	for (AEnemy* Enemy : AllEnemyPool)
	{
		if (Enemy->GetMobState() == EMobState::EMS_InPool)
		{
			AvailableEnemy = Enemy;
			break;
		}
	}
	if (AvailableEnemy == nullptr)
	{
		AvailableEnemy = GetWorld()->SpawnActor<AEnemy>(
			EnemyClass,
			FVector(0, 1000, -10000),
			FRotator::ZeroRotator
			);
		AvailableEnemy->SetIsPooling(true);
		AllEnemyPool.AddUnique(AvailableEnemy);
	}
	AvailableEnemy->SetMobState(EMobState::EMS_Initializing);
	if (bInIsRandom)
	{
		ActiveEnemyPool.Add(AvailableEnemy);
		int32 RandomID = FMath::RandRange(0, AllowEnemyIDs.Num() - 1);
		AvailableEnemy->SpawnSetup(
			FName(*FString::FromInt(AllowEnemyIDs[RandomID])),
			true,
			InLocation,
			true,
			InDestination
			);
	}
	else
	{
		AvailableEnemy->SpawnSetup(
			FName(*FString::FromInt(InID)),
			false,
			InLocation,
			bInMoveToPlayer,
			InDestination
			);
	}
}

void ALevelManager::ContinuousMobs(int32 InAllowActiveMobNums, const TArray<int32>& InAllowEnemyIDs)
{
	AllowActiveMobNums = InAllowActiveMobNums;
	AllowEnemyIDs.Empty();
	AllowEnemyIDs = InAllowEnemyIDs;
	for (int32 i = 0; i < AllowActiveMobNums; i++)
	{
		PoolingMobs(true, 0, FVector(0, 0, 0), true, FVector(0, 0, 0));
	}
	GetWorldTimerManager().ClearTimer(TryRespawnTimerHandle);
	GetWorldTimerManager().SetTimer(TryRespawnTimerHandle, this, &ALevelManager::TryRespawnMoreMobs, .2f, true);
}

void ALevelManager::TryRespawnMoreMobs()
{
	if (ActiveEnemyPool.Num() < AllowActiveMobNums)
	{
		PoolingMobs(true, 0, FVector(0, 0, 0), true, FVector(0, 0, 0));
	}
}

void ALevelManager::RemoveEnemyFromActivePool(AEnemy* InEnemy)
{
	ActiveEnemyPool.Remove(InEnemy);
}

void ALevelManager::PoolingPickupBase(EPickupType InPickupType, const FVector& InLocation, int32 InExpValue)
{
	APickup* AvailablePickup = nullptr;
	for (APickup* Pickup : PickupPool)
	{
		if (Pickup->GetPickupState() == EPickupState::EPS_InPool)
		{
			AvailablePickup = Pickup;
			break;
		}
	}
	if (!AvailablePickup)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AvailablePickup = GetWorld()->SpawnActor<APickup>(
			PickupClass,
			FVector(0, 1000, -10000),
			FRotator::ZeroRotator,
			SpawnParameters
			);
		PickupPool.AddUnique(AvailablePickup);
	}
	AvailablePickup->InitializePickup(InPickupType, InExpValue);
	AvailablePickup->SetActorLocation(InLocation);
}

void ALevelManager::TimeCount()
{
	PlayGameTimer++;
	int32 second = PlayGameTimer % 60;
	int32 minute = PlayGameTimer / 60;
	if (UVampireGameInstance* VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance()))
	{
		if (AVampire2DPlayerController* Vampire2DPlayerController = Cast<AVampire2DPlayerController>(VampireGameInstance->PlayerController))
		{
			Vampire2DPlayerController->GetGameLayout()->UpdateTime(minute, second);
			
		}
	}
	CallTimerFunctions();
}

void ALevelManager::ClearTimer()
{
	GetWorldTimerManager().ClearTimer(TryRespawnTimerHandle);
	GetWorldTimerManager().ClearTimer(TimeCountTimerHandle);
	TryRespawnTimerHandle.Invalidate();
	TimeCountTimerHandle.Invalidate();
}

void ALevelManager::CalculateFinalScore()
{
	int32 KillScore = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->GetKillCount() * 50;
	int32 TimeScore = PlayGameTimer * 10;
	int32 Level = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->GetCurLevel();
	FinalScore = KillScore + TimeScore + Level * 100;
	CalculateCoins(FinalScore);
}

void ALevelManager::CalculateCoins(int32 InFinalScore)
{
	int32 Increment = FMath::TruncToInt32(InFinalScore / FMath::RandRange(9.0f, 10.0f));
	int32 FinalCoin = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->AddCoins(Increment);
	SaveCoinData();
	UGameInstanceFunctionLibrary::GetGameLayout(this)->UpdateGameOverUI(FinalScore, FinalCoin);
}

void ALevelManager::MobPatterns(const EPatternTypes& InPattern, int32 InMaxLoop, int32 InSpecifiedID)
{
	AllowEnemyIDs = {InSpecifiedID};
	FVector SpawnLocation = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->PlayerCharacter->GetActorLocation(); 
	switch (InPattern)
	{
		case EPatternTypes::EPT_CROSS:
		{
			AMobPatternBase* CrossPattern = GetWorld()->SpawnActor<AMobPatternBase>(CrossPatternClass, SpawnLocation, FRotator::ZeroRotator);
			CrossPattern->Initialize(InSpecifiedID);
			break;
		}
		case EPatternTypes::EPT_CIRCLE:
		{
			AMobPatternBase* CirclePattern = GetWorld()->SpawnActor<AMobPatternBase>(CirclePatternClass, SpawnLocation, FRotator::ZeroRotator);
			CirclePattern->Initialize(InSpecifiedID);
			break;
		}
		case EPatternTypes::EPT_GRID:
		{
			AGridPattern* GridPattern = Cast<AGridPattern>(GetWorld()->SpawnActor<AMobPatternBase>(GridPatternClass, SpawnLocation, FRotator::ZeroRotator));
			GridPattern->PreInitialize(InMaxLoop);
			GridPattern->Initialize(InSpecifiedID);
			break;
		}
		default: break;
	}
}

// Allow spawning random different mobs at the sametime
void ALevelManager::MobBurst(const TArray<int32>& InAllowMobsID, int32 InRepeatedLoops)
{
	AllowEnemyIDs.Empty();
	AllowEnemyIDs = InAllowMobsID;
	for (int32 i = 0; i < InRepeatedLoops; i++)
	{
		PoolingMobs(true, 0, FVector::ZeroVector, true);
	}
}

// Use for pattern mobs or BOSS
void ALevelManager::UnpoolingMobs(int32 InID, const FVector& InScale) const
{
	FActorSpawnParameters SpawnParameters;
	AEnemy* SpawnMob = GetWorld()->SpawnActor<AEnemy>(
		EnemyClass,
		FVector(0, 0, -9990),
		FRotator::ZeroRotator,
		SpawnParameters
		);
	SpawnMob->SetActorScale3D(InScale);
	SpawnMob->SpawnSetup(
		FName(FString::FromInt(InID)),
		true,
		FVector::ZeroVector,
		true,
		FVector::ZeroVector
		);
}

void ALevelManager::KillAllMobs()
{
	ContinuousMobs(0,{0});
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(this, EnemyClass, AllEnemies);
	for (AActor* Enemy : AllEnemies)
	{
		Cast<AEnemy>(Enemy)->ForceDeath();
	}
}

void ALevelManager::RandomEvent()
{
	UGameInstanceFunctionLibrary::GetGameLayout(this)->WS_InGame->SetActiveWidgetIndex(2);
	Test::Shuffle(RandomEvents);
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	GameInstance->RandomEffectTag = RandomEvents[0];
	for (FShopItemInfo& LItemInfo : GameInstance->ShopItemInfos)
	{
		if (LItemInfo.UpgradeTag == GameInstance->RandomEffectTag)
		{
			CalcAndInitRandomBuff(LItemInfo);
		}
	}
}

void ALevelManager::CalcAndInitRandomBuff(FShopItemInfo& LItemInfo)
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	FText BuffDesc;
	FString StrA;
	FString StrB;
	if (GameInstance->RandomEffectTag == "rss_randomatk")
	{
		StrA = "ATK";
		StrB = FString::FromInt(LItemInfo.CurrentLv * 3);
	}
	else if (GameInstance->RandomEffectTag == "rss_randomcritprobability")
	{
		StrA = "Critical probability";
		StrB = FString::FromInt(LItemInfo.CurrentLv * 2);
	}
	else if (GameInstance->RandomEffectTag == "rss_randompickup")
	{
		StrA = "Pickup radius";
		StrB = FString::FromInt(LItemInfo.CurrentLv * 3);
	}
	else if (GameInstance->RandomEffectTag == "rss_randomspd")
	{
		StrA = "Moving speed";
		StrB = FString::FromInt(LItemInfo.CurrentLv * 2);
	}
	else if (GameInstance->RandomEffectTag == "rss_randomdef")
	{
		StrA = "Defence";
		StrB = FString::FromInt(LItemInfo.CurrentLv * 2);
	}
	if (LItemInfo.CurrentLv == 0)
	{
		BuffDesc = FText::FromString("No effects");
	}
	else
	{
		BuffDesc = FText::Format(FTextFormat::FromString("{0} is increased by {1}%."), FText::FromString(StrA), FText::FromString(StrB));
	}
	
	UGameInstanceFunctionLibrary::GetGameLayout(this)->InitRandomBuff(
		LItemInfo.Icon,
		FText::AsNumber(LItemInfo.CurrentLv),
		FText::FromString(StrA),
		BuffDesc
		);
}

void ALevelManager::SaveCoinData()
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	GameInstance->GameplaySaves->Coins = GameInstance->Coins;
	UGameplayStatics::SaveGameToSlot(GameInstance->GameplaySaves, GameInstance->SaveSlots, 0);
}

void ALevelManager::CallTimerFunctions()
{
	switch (PlayGameTimer)
	{
	case 1:
		Sec1();
		break;
	case 2:
		Sec2();
		break;
	case 10:
		Sec10();
		break;
	case 35:
		Sec35();
		break;
	case 45:
		Sec45();
		break;
	case 50:
		Sec50();
		break;
	case 60:
		Sec60();
		break;
	case 65:
		Sec65();
		break;
	case 85:
		Sec85();
		break;
	case 95:
		Sec95();
		break;
	case 101:
		Sec101();
		break;
	case 120:
		Sec120();
		break;
	case 125:
		Sec125();
		break;
	case 140:
		Sec140();
		break;
	case 180:
		Sec180();
		break;
	case 200:
		Sec200();
		break;
	case 220:
		Sec220();
		break;
	case 240:
		Sec240();
		break;
	case 260:
		Sec260();
		break;
	case 280:
		Sec280();
		break;
	case 300:
		Sec300();
		break;
	case 315:
		Sec315();
		break;
	case 340:
		Sec340();
		break;
	case 360:
		Sec360();
		break;
	case 370:
		Sec370();
		break;
	case 390:
		Sec390();
		break;
	case 425:
		Sec425();
		break;
	case 455:
        Sec455();
        break;
	case 460:
		Sec460();
		break;
	case 480:
		Sec480();
		break;
	case 500:
		Sec500();
		break;
	case 505:
		Sec505();
		break;
	case 520:
		Sec520();
		break;
	case 540:
		Sec540();
		break;
	case 580:
		Sec580();
		break;
	case 620:
		Sec620();
		break;
	case 650:
        Sec650();
        break;
	case 690:
		Sec690();
		break;
	case 720:
		Sec720();
		break;
	case 750:
		Sec750();
		break;
	case 790:
		Sec790();
		break;
	case 820:
		Sec820();
		break;
	case 840:
		Sec840();
		break;
	case 865:
		Sec865();
		break;
	case 900:
		Sec900();
		break;
	case 950:
		Sec950();
		break;
	case 955:
		Sec955();
		break;
	case 960:
        Sec960();
        break;
	case 1000:
		Sec1000();
		break;
	case 1030:
		Sec1030();
		break;
	case 1050:
		Sec1050();
		break;
	case 1090:
		Sec1090();
		break;
	case 1200:
		Sec1200();
		break;
	case 1205:
		Sec1205();
		break;
	case 1215:
		Sec1215();
		break;
	case 1500:
		Sec1500();
		break;
		default: break;
	}
}

void ALevelManager::Sec1()
{
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->BGMManagement(NormalBGM);
}

void ALevelManager::Sec2()
{
	ContinuousMobs(8, {1});
}

void ALevelManager::Sec10()
{
	MobBurst({1,2}, 35);
}

void ALevelManager::Sec35()
{
	ContinuousMobs(35, {1,2});
}

void ALevelManager::Sec45()
{
	MobPatterns(EPatternTypes::EPT_CROSS, 1, 2);
}

void ALevelManager::Sec50()
{
	MobBurst({1,2}, 15);
}

void ALevelManager::Sec60()
{
	MobBurst({1}, 10);
}

void ALevelManager::Sec65()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 1);
}

void ALevelManager::Sec85()
{
	ContinuousMobs(35, {2});
}

void ALevelManager::Sec95()
{
	MobPatterns(EPatternTypes::EPT_GRID, 3, 501);
}

void ALevelManager::Sec101()
{
	MobBurst({1}, 20);
	MobBurst({2}, 20);
	ContinuousMobs(35,{1,2});
}

void ALevelManager::Sec120()
{
	UnpoolingMobs(3, FVector(3));
}

void ALevelManager::Sec125()
{
	ContinuousMobs(45, {1,2});
}

void ALevelManager::Sec140()
{
	MobPatterns(EPatternTypes::EPT_GRID, 1, 501);
}

void ALevelManager::Sec180()
{
	ContinuousMobs(30, {2,4});
}

void ALevelManager::Sec200()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 502);
	ContinuousMobs(40, {4});
}

void ALevelManager::Sec220()
{
	MobPatterns(EPatternTypes::EPT_GRID, 5, 501);
	ContinuousMobs(55, {2,4});
}

void ALevelManager::Sec240()
{
	MobPatterns(EPatternTypes::EPT_GRID, 5, 502);
}

void ALevelManager::Sec260()
{
	ContinuousMobs(40, {2,4,5});
	UnpoolingMobs(6, FVector(3));
}

void ALevelManager::Sec280()
{
	MobPatterns(EPatternTypes::EPT_CROSS, 1, 502);
	ContinuousMobs(40, {4,5});
}

void ALevelManager::Sec300()
{
	MobPatterns(EPatternTypes::EPT_GRID, 10, 502);
}

void ALevelManager::Sec315()
{
	ContinuousMobs(60, {7,8,9});
}

void ALevelManager::Sec340()
{
	ContinuousMobs(40, {7});
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1,503);
}

void ALevelManager::Sec360()
{
	ContinuousMobs(60, {7,10});
	MobPatterns(EPatternTypes::EPT_GRID, 5, 503);
}

void ALevelManager::Sec370()
{
	UnpoolingMobs(11, FVector(3));
	ContinuousMobs(60, {7,8,9,10});
}

void ALevelManager::Sec390()
{
	ContinuousMobs(50, {7,8,9,10,12});
}

void ALevelManager::Sec425()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 5, 503);
	ContinuousMobs(40, {10,12});
}

void ALevelManager::Sec455()
{
	MobPatterns(EPatternTypes::EPT_CROSS, 1, 504);
	ContinuousMobs(75, {10,12,13});
}

void ALevelManager::Sec460()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 505);
}

void ALevelManager::Sec480()
{
	UnpoolingMobs(14, FVector(3));
}

void ALevelManager::Sec500()
{
	MobPatterns(EPatternTypes::EPT_GRID, 3, 504);
}

void ALevelManager::Sec505()
{
	MobPatterns(EPatternTypes::EPT_GRID, 3, 505);
}

void ALevelManager::Sec520()
{
	MobBurst({15},5);
}

void ALevelManager::Sec540()
{
	ContinuousMobs(55, {12,13,15});
}

void ALevelManager::Sec580()
{
	MobPatterns(EPatternTypes::EPT_GRID, 10, 506);
}

void ALevelManager::Sec620()
{
	UnpoolingMobs(16, FVector(4));
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->BGMManagement(BossBGM);
}

void ALevelManager::Sec650()
{
	ContinuousMobs(55, {15,17});
	MobBurst({17}, 40);
}

void ALevelManager::Sec690()
{
	MobPatterns(EPatternTypes::EPT_GRID, 5, 507);
	ContinuousMobs(75, {17,18});
}

void ALevelManager::Sec720()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 507);
	ContinuousMobs(55, {17,18});
}

void ALevelManager::Sec750()
{
	ContinuousMobs(85, {18,20});
}

void ALevelManager::Sec790()
{
	MobPatterns(EPatternTypes::EPT_GRID, 20, 507);
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->BGMManagement(NormalBGM);
}

void ALevelManager::Sec820()
{
	UnpoolingMobs(19, FVector(3));
	ContinuousMobs(75, {17,20});
}

void ALevelManager::Sec840()
{
	ContinuousMobs(75, {21,24});
}

void ALevelManager::Sec865()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 508);
	MobBurst({21,24}, 25);
}

void ALevelManager::Sec900()
{
	UnpoolingMobs(22, FVector(3));
	UnpoolingMobs(23, FVector(3));
	ContinuousMobs(75, {24});
	MobBurst({24}, 25);
}

void ALevelManager::Sec950()
{
	ContinuousMobs(75, {24,25});
	MobPatterns(EPatternTypes::EPT_CROSS, 1, 509);
}

void ALevelManager::Sec955()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 509);
}

void ALevelManager::Sec960()
{
	MobPatterns(EPatternTypes::EPT_CIRCLE, 1, 509);
}

void ALevelManager::Sec1000()
{
	UnpoolingMobs(26, FVector(3));
	ContinuousMobs(100, {27});
}

void ALevelManager::Sec1030()
{
	MobPatterns(EPatternTypes::EPT_GRID, 1, 510);
	MobBurst({28}, 1);
	MobBurst({29}, 1);
}

void ALevelManager::Sec1050()
{
	MobPatterns(EPatternTypes::EPT_GRID, 5, 510);
}

void ALevelManager::Sec1090()
{
	ContinuousMobs(100, {28,29});
}

void ALevelManager::Sec1200()
{
	KillAllMobs();
}

void ALevelManager::Sec1205()
{
	UnpoolingMobs(31, FVector(4));
	UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->BGMManagement(BossBGM);

}

void ALevelManager::Sec1215()
{
	ContinuousMobs(40,{30});
}

void ALevelManager::Sec1500()
{
	MobBurst({30}, 40);
}


