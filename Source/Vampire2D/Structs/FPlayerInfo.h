#pragma once
#include "FPlayerInfo.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FPlayerInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK_mul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SPD_mul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CRT_mul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurEXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PickupRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Haste_mul = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> UniqueSkillTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InitialSkillTag;
};
