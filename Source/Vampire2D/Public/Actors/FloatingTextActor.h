// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingTextActor.generated.h"

class UWidgetComponent;

UCLASS()
class VAMPIRE2D_API AFloatingTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingTextActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* FloatingTextWidget;

	USceneComponent* SceneComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void DisplayFloatingText(const FText& inText, const FLinearColor& inColor) const;
};
