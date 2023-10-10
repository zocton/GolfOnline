// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TalentBase.generated.h"

/**
 * 
 */
UCLASS()
class GOLFONLINE_API UTalentBase : public UObject
{
	GENERATED_BODY()
public:
	FString GetTalentName() { return Name; }
protected:
	UPROPERTY(EditAnywhere)
	FString Name;
};
