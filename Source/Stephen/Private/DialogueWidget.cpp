// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

void UDialogueWidget::Output_Set(const FString& Sent_Text)
{
    this->Output_Text = Sent_Text;
}

void UDialogueWidget::Output_Append(const TCHAR Sent_Char)
{
    this->Output_Text.FString::AppendChar(Sent_Char);
}

void UDialogueWidget::Output_Append(const FString& Sent_Text)
{
    this->Output_Text.FString::Append(Sent_Text);
}