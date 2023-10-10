// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/WidgetManager.h"
#include "UI/Talents/TalentMenu.h"

void UWidgetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWidgetManager::Deinitialize()
{
	Super::Deinitialize();
}

void UWidgetManager::OpenInGameMenu(EMenuType MenuType)
{
	switch (MenuType)
	{
	case EMenuType::Talents:
		CreateWidget<UTalentMenu>(GetGameInstance()->GetFirstLocalPlayerController(), BP_TalentMenu.LoadSynchronous())->AddToViewport();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Menu creation called with unknown enumeration value."));
		break;
	}
}
