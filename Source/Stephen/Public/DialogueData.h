// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	FDialogueData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CharNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText BlockText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Respond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName NextBlockName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> Responses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> Delegates;

};
