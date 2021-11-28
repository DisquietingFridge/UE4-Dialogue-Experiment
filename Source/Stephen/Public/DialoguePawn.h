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
	
	// Pertaining to data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UDataTable* Lines_Table;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Line;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Nametag;
	
	// Pertaining to original player controller
	UPROPERTY(BlueprintReadWrite) class APawn* Repossess_Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Controller_Rotation;

	// Pertaining to widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UDialogueWidget> Window_Class;
	UPROPERTY(BlueprintReadWrite) UDialogueWidget* Dialogue_Window;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Scan_Delay;
	TCHAR CharMessage;
	UPROPERTY(BlueprintReadWrite) FText Block_Text;

	// Delegates
	DECLARE_MULTICAST_DELEGATE(GenericEvent)
	GenericEvent AppendChar;
	
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


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
