// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueStructs.h"
#include "Engine/DataTable.h"
#include "DialoguePawn.h"
#include "DialogueWidget.h"

// Sets default values
ADialoguePawn::ADialoguePawn()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	AppendChar.AddUObject(this, &ADialoguePawn::Drive_Append_Char);
}


 void ADialoguePawn::initDialoguePawn(
	 FTalkerStruct Talker_Info,
	 FRotator Controller_Rotation_in,
	 class APawn* Repossess_Target_in) {

	 // init variables
	 Lines_Table = Talker_Info.Lines_Table;
	 Nametag = Talker_Info.Nametag;
	 Controller_Rotation = Controller_Rotation_in;
	 Repossess_Target = Repossess_Target_in;
	 Next_Block_Name = Talker_Info.First_Block_Name;

	 //kick off the first block of text
	 Next_Block();
 }

 void ADialoguePawn::Drive_Append_Char()
 {
	 if (Dialogue_Window) Dialogue_Window->Output_Append(Block_Text[charIndex]);
 }

 void ADialoguePawn::Drive_Set_Text()
 {
	Dialogue_Window->Output_Set(Block_Text);
 }

 void ADialoguePawn::Next_Block_Implementation()
 {
	 //initialize things for new block
	 Dialogue_Window->Output_Set("");
	 charIndex = 0;

	 //import and process table row
	 FDialogueData* imported = Lines_Table->FindRow<FDialogueData>(Next_Block_Name, "");
	 Process_Row(imported);

	 //start timer
	 GetWorldTimerManager().SetTimer(DialogueScanTimer, this, &ADialoguePawn::TimerFired, Scan_Period, true, Scan_Period);

	 Skip_Or_Next.BindUObject(this, &ADialoguePawn::Skip_Block);
 }

 void ADialoguePawn::Skip_Block_Implementation()
 {
	 GetWorldTimerManager().ClearTimer(DialogueScanTimer);

	 Skip_Or_Next.BindUObject(this, &ADialoguePawn::Next_Block);

	 Drive_Set_Text();

 }

 void ADialoguePawn::Kill_Dialogue_Implementation() {}


 void ADialoguePawn::Process_Row(FDialogueData* imported) {

	 if (imported) { // if row was found
		 Next_Block_Name = imported->NextBlockName;
		 Block_Text = (imported->BlockText).ToString();
		 blocklen = Block_Text.Len();
	 }
	 else { // else exit dialogue
		 Kill_Dialogue();
	 }
 }

 void ADialoguePawn::TimerFired()
 {
	 if (charIndex < blocklen) {
		 AppendChar.Broadcast();
		 charIndex++;
	 }
	 else {
		 GetWorldTimerManager().ClearTimer(DialogueScanTimer);
		 Skip_Or_Next.BindUObject(this, &ADialoguePawn::Next_Block_Implementation);
	 }

 }

 void ADialoguePawn::FireDelegate()
 {
	 Skip_Or_Next.ExecuteIfBound();
 }



// Called when the game starts or when spawned
void ADialoguePawn::BeginPlay()
{
	Super::BeginPlay();

	//Dialogue window setup

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

