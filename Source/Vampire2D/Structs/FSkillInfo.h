#pragma once
#include "FSkillInfo.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SkillTag = "Skill";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillMaxLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* SkillIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName = FText::FromString("Skill");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> SkillDescriptions = { FText::FromString("Skill Description") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsNew = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsItem = false;
};
