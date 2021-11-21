// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "Templates/SharedPointer.h"

void UDialogueWidget::Send_Text_Implementation(const FString& Sent_Text)
{
    this->Output_Text = Sent_Text;
    return;
}

//returns smart pointer of text field to be driven
TSharedRef<FString> UDialogueWidget::GetFieldRef()
{
    return MakeShared<FString>(Output_Text);
}
