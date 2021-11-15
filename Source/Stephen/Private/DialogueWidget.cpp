// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

void UDialogueWidget::Send_Text_Implementation(const FString& Sent_Text)
{
    this->Output_Text = Sent_Text;
    return;
}

FString& UDialogueWidget::GetFieldRef_Implementation()
{
    return (this->Output_Text);
}