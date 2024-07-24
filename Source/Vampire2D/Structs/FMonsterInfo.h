#pragma once
#include "PaperFlipbook.h"
#include "FMonsterInfo.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FMonsterInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MonsterName = FText::FromString("Monster");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* MonsterFlipbook = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SPD = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EXP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropTreasureRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBoss = false;
};
