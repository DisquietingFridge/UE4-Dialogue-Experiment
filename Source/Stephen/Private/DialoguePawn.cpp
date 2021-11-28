// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueStructs.h"
#include "DialoguePawn.h"
#include "DialogueWidget.h"

// Sets default values
ADialoguePawn::ADialoguePawn()
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	Controller_Rotation = FRotator(0);
	Repossess_Target = nullptr; 
	Dialogue_Window = nullptr;
	Lines_Table = nullptr;
	Line = "";
	Nametag = FText::FromString("ERROR: UNDEFINED NAME");

	Dialogue_Window = nullptr;

	Scan_Delay = 0.005;
	CharMessage = CHAR(0);
	Block_Text = FText::FromString("");

	AppendChar.AddUObject(this, &ADialoguePawn::Drive_Append_Char);

}

 void ADialoguePawn::initDialoguePawn(
	 FTalkerStruct Talker_Info,
	 FRotator Controller_Rotation_in,
	 class APawn* Repossess_Target_in) {

	 Lines_Table = Talker_Info.Lines_Table;
	 Line = Talker_Info.First_Block_Name;
	 Nametag = Talker_Info.Nametag;
	 Controller_Rotation = Controller_Rotation_in;
	 Repossess_Target = Repossess_Target_in;
 }

 void ADialoguePawn::Drive_Append_Char()
 {
	 if (Dialogue_Window) Dialogue_Window->Output_Append(CharMessage);
 }

 void ADialoguePawn::Drive_Set_Text()
 {
	 Dialogue_Window->Output_Set(Block_Text.ToString());
 }

 void ADialoguePawn::Next_Block_Implementation()
 {
 }

 void ADialoguePawn::Skip_Block_Implementation()
 {
 }

// Called when the game starts or when spawned
void ADialoguePawn::BeginPlay()
{
	Super::BeginPlay();

	Dialogue_Window = NewObject<UDialogueWidget>(this, Window_Class);
	
	if (Dialogue_Window) {
		Dialogue_Window->AddToViewport(0);
		Dialogue_Window->SetVisibility(ESlateVisibility::Visible);
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

