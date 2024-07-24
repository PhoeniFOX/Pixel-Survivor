// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/GameInstanceFunctionLibrary.h"

#include "Data/Vampire2DPlayerController.h"
#include "Data/VampireGameInstance.h"
#include "Kismet/GameplayStatics.h"

UVampireGameInstance* UGameInstanceFunctionLibrary::GetVampireGameInstance(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UGameInstance* GameInstance = WorldContextObject->GetWorld()->GetGameInstance();
	if (!GameInstance) return nullptr;

	UVampireGameInstance* VampireGameInstance = Cast<UVampireGameInstance>(GameInstance);
	return VampireGameInstance;
}

bool UGameInstanceFunctionLibrary::CheckWithinScreen(const UObject* WorldContextObject, const FVector& WorldLocation)
{
	FVector2D ScreenLocation;
	UGameplayStatics::GetPlayerController(WorldContextObject, 0)->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation);
	if (ScreenLocation.X < 0 || ScreenLocation.X > GEngine->GameViewport->Viewport->GetSizeXY().X || ScreenLocation.Y < 0 || ScreenLocation.Y > GEngine->GameViewport->Viewport->GetSizeXY().Y)
	{
		return false;
	}
	return true;
}

UGameLayout* UGameInstanceFunctionLibrary::GetGameLayout(const UObject* WorldContextObject)
{
	AVampire2DPlayerController* Vampire2DPlayerController = Cast<AVampire2DPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	if (!Vampire2DPlayerController) return nullptr;
	return Vampire2DPlayerController->GetGameLayout();
}

// FVector UGameInstanceFunctionLibrary::GetRandomLocationInScreen(const UObject* WorldContextObject)
// {
// 	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
// 	int32 RandomX = FMath::RandRange(0, GEngine->GameViewport->Viewport->GetSizeXY().X);
// 	int32 RandomY = FMath::RandRange(0, GEngine->GameViewport->Viewport->GetSizeXY().Y);
// 	FVector WLocation;
// 	FVector WDirection;
// 	if (PlayerController->DeprojectScreenPositionToWorld(RandomX, RandomY, WLocation, WDirection))
// 	{
// 		return WLocation;
// 	}
// 	return FVector::ZeroVector;
// }
