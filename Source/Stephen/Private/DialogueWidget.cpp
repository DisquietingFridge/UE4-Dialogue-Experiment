// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

void UDialogueWidget::SetBlockData(FText Block)
{
	FinalText = Block.ToString();
}
//TODO: Allow this to support formatting characters
bool UDialogueWidget::RevealCharsAndIsDone(int amount)
{
	if (DisplayedText.Len() >= FinalText.Len())
	{
		return true;
	}

	for (int i = 0; i < amount; ++i)
	{
		DisplayedText.AppendChar(FinalText[DisplayedText.Len()]);

		if (DisplayedText.Len() == FinalText.Len())
		{
			return true;
		}
	}

	return false;
}

void UDialogueWidget::HideAllChars()
{
	DisplayedText = "";
}

void UDialogueWidget::RevealAllChars()
{
	DisplayedText = FinalText;
}


