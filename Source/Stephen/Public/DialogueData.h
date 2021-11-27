// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "Engine/DataTable.h"
#include "DialogueData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct STEPHEN_API FDialogueData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Char;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Respond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Next;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> Responses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> Delegates;

};
