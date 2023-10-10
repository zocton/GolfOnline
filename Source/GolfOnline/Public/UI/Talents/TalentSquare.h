// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TalentSquare.generated.h"

class UImage;
class UTexture2D;

/**
 * 
 */
UCLASS()
class GOLFONLINE_API UTalentSquare : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditInstanceOnly)
	UTexture2D* Texture;

protected:
	UPROPERTY(EditInstanceOnly, meta = (BindWidget))
	UImage* Image;
};
