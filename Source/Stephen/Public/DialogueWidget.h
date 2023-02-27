// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueInterface.h"
#include "DialogueWidget.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class STEPHEN_API UDialogueWidget : public UUserWidget, public IDialogueInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(Blueprintable) FString DisplayedText = "Output_Text initialized";
	UPROPERTY() FString FinalText;

public:

	void SetBlockData(FText Block) override;
	bool RevealCharsAndIsDone(int amount = 1) override;
	void HideAllChars() override;
	void RevealAllChars() override;

	

};