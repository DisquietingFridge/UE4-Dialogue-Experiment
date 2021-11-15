// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "Templates/SharedPointer.h"

void UDialogueWidget::Send_Text_Implementation(const FString& Sent_Text)
{
    this->Output_Text = Sent_Text;
    return;
}

/*TSharedRef<FString> UDialogueWidget::GetFieldRef()
{
    return MakeShared<FString>(this->Output_Text);
}*/

FString& UDialogueWidget::GetFieldRef() {
    return Output_Text;
}