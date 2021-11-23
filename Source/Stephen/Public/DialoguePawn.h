// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "DialogueWidget.h"
#include "DialoguePawn.generated.h"

UCLASS()
class STEPHEN_API ADialoguePawn : public APawn
{
	GENERATED_BODY()

public:

	TSharedRef<FString> DrivenText = MakeShared<FString>("TSharedRef initialized");
	
	// Pertaining to data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite) class UDataTable* Lines_Table;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Line;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Nametag;
	
	// Pertaining to original player controller
	UPROPERTY(BlueprintReadWrite) class APawn* Repossess_Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Controller_Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UDialogueWidget> Window_Class;
	UPROPERTY(BlueprintReadWrite) UDialogueWidget* Dialogue_Window;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Scan_Delay;
	UPROPERTY(BlueprintReadWrite) FString Inprogress_String;
	UPROPERTY(BlueprintReadWrite) FText Block_Text;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) //, BlueprintImplementableEvent
	void initDialoguePawn(
		class UDataTable* Lines_Table_in,
		FName Line_in,
		const FText& Nametag_in,
		FRotator Controller_Rotation_in,
		class APawn* Repossess_Target_in);

	virtual void initDialoguePawn_Implementation(
		class UDataTable* Lines_Table_in,
		FName Line_in,
		const FText& Nametag_in,
		FRotator Controller_Rotation_in,
		class APawn* Repossess_Target_in);


	// Sets default values for this pawn's properties
	ADialoguePawn();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
