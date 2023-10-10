// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TalentRow.generated.h"

class UHorizontalBox;

/**
 * 
 */
UCLASS()
class GOLFONLINE_API UTalentRow : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox;
};
