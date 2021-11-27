// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "DialogueInterface.h"
#include "DialogueWidget.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class STEPHEN_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

private:

	UPROPERTY()
		FString Output_Text = "Output_Text initialized";

public:


	void Output_Set(const FString& Sent_Text);


	// Either append a single character or a string
	void Output_Append(const TCHAR Sent_Char);
	void Output_Append(const FString& Sent_Text);

};