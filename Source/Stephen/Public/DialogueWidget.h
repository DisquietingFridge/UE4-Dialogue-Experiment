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

	//https://nerivec.github.io/old-ue4-wiki/pages/how-to-modify-blueprint-variable-references-in-c-without-copying.html
	//

	virtual const FString& GetFieldRef() const override;
	//virtual const FString& GetFieldRef_Implementation() const override;
	//virtual TSharedRef<FString> GetFieldRef() override;
	//virtual TSharedRef<FString> GetFieldRef_Implementation() override;

};
