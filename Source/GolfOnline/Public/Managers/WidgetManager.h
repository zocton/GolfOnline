// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WidgetManager.generated.h"

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	Talents
};

class UTalentMenu;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GOLFONLINE_API UWidgetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void OpenInGameMenu(EMenuType MenuType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftClassPtr<UTalentMenu> BP_TalentMenu;
};
