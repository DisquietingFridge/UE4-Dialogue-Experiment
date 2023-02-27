// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UDialogueWidget;
class UDataTable;


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueStructs.h"
#include "DialogueDriver.generated.h"

UENUM(Blueprintable)
enum DialogueState
{
	BlockInProgress,
	BlockFinished,
	AwaitingChoice
};

UCLASS(Blueprintable)
class STEPHEN_API UDialogueDriver : public UActorComponent
{
	GENERATED_BODY()

public:

	int32 charIndex = 0;
	int32 blocklen = 0;
	FTimerHandle DialogueScanTimer;
	
	// Pertaining to data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UDataTable* Lines_Table = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName First_Block_Name;
	UPROPERTY(BlueprintReadWrite) FName Next_Block_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Nametag = FText::FromString("ERROR: UNDEFINED NAME");

	// Pertaining to widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UDialogueWidget> Window_Class;
	UPROPERTY(BlueprintReadWrite) UDialogueWidget* Dialogue_Window = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Scan_Period = 0.005;
	UPROPERTY(BlueprintReadWrite) FString Block_Text;

	// Sets default values for this pawn's properties
	UDialogueDriver();

protected:

	DialogueState CurrentState;
	FDialogueData CurrentData;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void SetupInput();

	UFUNCTION(BlueprintCallable)
		void StartDialogue();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StartNewBlock(FName NextBlockName);
	virtual void StartNewBlock_Implementation(FName NextBlockName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void FinishBlock();
	virtual void FinishBlock_Implementation();

	UFUNCTION()
		void TimerFired();

	UFUNCTION(BlueprintCallable)
		void DialogueInteractReceived();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EndDialogue();
	virtual void EndDialogue_Implementation();


	void Process_Row(FDialogueData* imported);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
