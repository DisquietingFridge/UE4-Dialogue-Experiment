// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogueInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDialogueInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEPHEN_API IDialogueInterface
{
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	GENERATED_BODY()

public:
	//transmits text to a recipient. Likely to be deprecated.
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Blueprintable)
		void Send_Text(const FString& Sent_Text);

	// asks subject to return the reference of the FString field it would like "driven".
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable)
		FString& GetFieldRef();
};
