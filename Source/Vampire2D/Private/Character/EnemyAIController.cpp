// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyAIController.h"

#include "Character/Enemy.h"
#include "Navigation/PathFollowingComponent.h"

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (Result.IsFailure())
	{
		if (AEnemy* Enemy = Cast<AEnemy>(GetPawn()))
		{
			if (Enemy->GetIsMoveToPlayer())
			{
				Enemy->MoveToPlayerFail();
			}
			else
			{
				Enemy->MoveToLocationFail();
			}
		}
	} else if (Result.IsSuccess())
	{
		if (AEnemy* Enemy = Cast<AEnemy>(GetPawn()))
		{
			if (Enemy->GetIsMoveToPlayer())
			{
				Enemy->MoveToPlayerSuccess();
				Enemy->ResetMoving();
			}
			else
			{
				Enemy->MoveToLocationSuccess();
			}
		}
	}
}
