// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
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

	virtual void SetBlockData(FText Block) {}
	virtual bool RevealCharsAndIsDone(int amount = 1) { return true; }
	virtual void HideAllChars() {}
	virtual void RevealAllChars() {}


};
