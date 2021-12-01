// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AAutomataDriver;
#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "AutomataCell.generated.h"

UENUM()
enum ECellState
{
	On UMETA(DisplayName = "On"),
	Off UMETA(DisplayName = "Off"),
};


UCLASS()
class STEPHEN_API UAutomataCell : public UStaticMeshComponent
{
	GENERATED_BODY()

public:	

	// "Owner" of this cell. Will define rulesets, drive transitions, etc
	AAutomataDriver* Driver;

	// references to neighbours' cell states that will determine next state
	TArray<TEnumAsByte<ECellState>*> Neighbour_States;

	// Cell's previously set state
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECellState> PrevState = Off;

	// State that cell will next switch to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECellState> NextState = Off;

	// update state
	UFUNCTION(BlueprintCallable)
		void Cell_Transition();

	//calculate new state
	UFUNCTION(BlueprintCallable)
		void Calc_Next();
		
};


