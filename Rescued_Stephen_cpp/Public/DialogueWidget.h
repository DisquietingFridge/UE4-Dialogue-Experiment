// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Templates/SharedPointer.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Output_Text = "";
	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable)
		void Send_Text(const FString& Sent_Text);
	virtual void Send_Text_Implementation(const FString& Sent_Text) override;

	//returns smart pointer of text field to be driven
	//smart pointers cannot be exposed to reflection / blueprints (i.e. no UFUNCTION declaration)
	virtual TSharedRef<FString> GetFieldRef() override;

};
