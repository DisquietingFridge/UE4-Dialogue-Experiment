// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueInterface.h"

// Add default functionality here for any IDialogueInterface functions that are not pure virtual.

FString& IDialogueInterface::GetFieldRef()
{
    static FString perm = "";
    return perm;
}

