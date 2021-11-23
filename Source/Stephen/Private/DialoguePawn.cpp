// Fill out your copyright notice in the Description page of Project Settings.


#include "DialoguePawn.h"
//#include "GameFramework/Controller.h"

// Sets default values
ADialoguePawn::ADialoguePawn()
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	Controller_Rotation = FRotator(0);
	Repossess_Target = nullptr; 
	Dialogue_Window = nullptr;
	Lines_Table = nullptr;
	Line = "";
	Nametag = FText::FromString("");

	Dialogue_Window = nullptr;

	Scan_Delay = 0.005;
	Inprogress_String = "";
	Block_Text = FText::FromString("");
}

 void ADialoguePawn::initDialoguePawn_Implementation(
	 class UDataTable* Lines_Table_in,
	 FName Line_in,
	 const FText& Nametag_in,
	 FRotator Controller_Rotation_in,
	 class APawn* Repossess_Target_in) {

	 Lines_Table = Lines_Table_in;
	 Line = Line_in;
	 Nametag = Nametag_in;
	 Controller_Rotation = Controller_Rotation_in;
	 Repossess_Target = Repossess_Target_in;
 }

// Called when the game starts or when spawned
void ADialoguePawn::BeginPlay()
{
	Super::BeginPlay();

	Dialogue_Window = NewObject<UDialogueWidget>(this, Window_Class);
	
	if (Dialogue_Window) {
		Dialogue_Window->AddToViewport(0);
		DrivenText = Dialogue_Window->GetFieldRef();
		//(*DrivenText) = "DrivenText assigned to";
		Dialogue_Window->Output_Text = "Manual Assignment Successful";
		Dialogue_Window->Output_Text = (*DrivenText);
	}


}

// Called every frame
void ADialoguePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADialoguePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

