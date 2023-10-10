// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelGenerationManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GOLFONLINE_API ULevelGenerationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSoftClassPtr<AActor>> BP_ObjectCollection;
};
