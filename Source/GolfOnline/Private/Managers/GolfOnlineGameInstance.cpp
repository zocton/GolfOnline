// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GolfOnlineGameInstance.h"
#include "Managers/LevelGenerationManager.h"

void UGolfOnlineGameInstance::Init()
{
	Super::Init();
}

void UGolfOnlineGameInstance::GenerateLevel()
{
	UE_LOG(LogTemp, Display, TEXT("level object count: %d"), GetSubsystem<ULevelGenerationManager>()->BP_ObjectCollection.Num());
}
