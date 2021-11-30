// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UDialogueWidget;

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "DialogueStructs.h"
#include "DialoguePawn.generated.h"

UCLASS()
class STEPHEN_API ADialoguePawn : public APawn
{
	GENERATED_BODY()

public:

	int32 charIndex = 0;
	int32 blocklen = 0;
	FTimerHandle DialogueScanTimer;
	
	// Pertaining to data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UDataTable* Lines_Table = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Next_Block_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Nametag = FText::FromString("ERROR: UNDEFINED NAME");
	
	// Pertaining to original player controller
	UPROPERTY(BlueprintReadWrite) class APawn* Repossess_Target = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Controller_Rotation = FRotator(0);

	// Pertaining to widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UDialogueWidget> Window_Class;
	UPROPERTY(BlueprintReadWrite) UDialogueWidget* Dialogue_Window = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Scan_Period = 0.005;
	UPROPERTY(BlueprintReadWrite) FString Block_Text = "";

	// Delegates
	DECLARE_MULTICAST_DELEGATE(GenericEvent)
	GenericEvent AppendChar;

	//UDELEGATE(BlueprintCallable)
	DECLARE_DELEGATE(ButtonResponse)
	ButtonResponse Skip_Or_Next;

protected:



public:

	
	UFUNCTION(BlueprintCallable)
		void initDialoguePawn(
			FTalkerStruct Talker_Info,
			FRotator Controller_Rotation_in,
			class APawn* Repossess_Target_in);

	// Sets default values for this pawn's properties
	ADialoguePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void Drive_Append_Char();

	UFUNCTION(BlueprintCallable)
		void Drive_Set_Text();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Next_Block();
	virtual void Next_Block_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Skip_Block();
	virtual void Skip_Block_Implementation();

	UFUNCTION()
		void TimerFired();

	UFUNCTION(BlueprintCallable)
		void FireDelegate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Kill_Dialogue();
	virtual void Kill_Dialogue_Implementation();


	void Process_Row(FDialogueData* imported);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
