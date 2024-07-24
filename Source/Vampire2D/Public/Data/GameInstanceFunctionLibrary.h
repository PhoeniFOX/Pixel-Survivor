// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameInstanceFunctionLibrary.generated.h"

class UGameLayout;
class UVampireGameInstance;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API UGameInstanceFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static UVampireGameInstance* GetVampireGameInstance(const UObject* WorldContextObject);
	static bool CheckWithinScreen(const UObject* WorldContextObject, const FVector& WorldLocation);
	static UGameLayout* GetGameLayout(const UObject* WorldContextObject);
	static FVector GetRandomLocationInScreen(const UObject* WorldContextObject);
};
