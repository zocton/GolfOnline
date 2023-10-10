// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GolfOnlineGameInstance.generated.h"

class ULevelGenerationManager;

/**
 * 
 */
UCLASS()
class GOLFONLINE_API UGolfOnlineGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable) void GenerateLevel();

	UPROPERTY(EditDefaultsOnly) TSoftClassPtr<ULevelGenerationManager> BP_LevelGenerationManager;
};
