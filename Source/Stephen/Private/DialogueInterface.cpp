// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueInterface.h"

// Add default functionality here for any IDialogueInterface functions that are not pure virtual.


//default functionality that won't actually be used- just make a static variable who cares
TSharedRef<FString> IDialogueInterface::GetFieldRef()
{
    static FString perm = "Default Interface Implementation";
    return MakeShared<FString>(perm);
}

