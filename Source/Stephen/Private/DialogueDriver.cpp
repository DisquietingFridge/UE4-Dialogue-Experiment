// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueDriver.h"
#include "DialogueStructs.h"
#include "Engine/DataTable.h"
#include "DialogueWidget.h"

// Sets default values
UDialogueDriver::UDialogueDriver()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

 void UDialogueDriver::Drive_Append_Char()
 {
	 if (Dialogue_Window) Dialogue_Window->Output_Append(Block_Text[charIndex]);
 }

 void UDialogueDriver::Drive_Set_Text()
 {
	Dialogue_Window->Output_Set(Block_Text);
 }

 void UDialogueDriver::Next_Block_Implementation()
 {
	 //initialize things for new block
	 Dialogue_Window->Output_Set("");
	 charIndex = 0;

	 //import and process table row
	 FDialogueData* imported = Lines_Table->FindRow<FDialogueData>(Next_Block_Name, "");
	 Process_Row(imported);

	 //start timer
	 GetOwner()->GetWorldTimerManager().SetTimer(DialogueScanTimer, this, &UDialogueDriver::TimerFired, Scan_Period, true, Scan_Period);

	 CurrentState = BlockInProgress;
 }

 void UDialogueDriver::Skip_Block_Implementation()
 {
	 GetOwner()->GetWorldTimerManager().ClearTimer(DialogueScanTimer);

	 CurrentState = BlockFinished;

	 Drive_Set_Text();

 }

 void UDialogueDriver::Kill_Dialogue_Implementation() 
 {
	 if (Dialogue_Window) {
		 Dialogue_Window->RemoveFromViewport();
		 Dialogue_Window = nullptr; // remove for garbage collection
	 }

	 GetOwner()->DisableInput(GEngine->GetFirstLocalPlayerController(GetWorld()));
 }


 void UDialogueDriver::Process_Row(FDialogueData* imported) {

	 if (imported) { // if row was found
		 Next_Block_Name = imported->NextBlockName;
		 Block_Text = (imported->BlockText).ToString();
		 blocklen = Block_Text.Len();
	 }
	 else { // else exit dialogue
		 Kill_Dialogue_Implementation();
	 }
 }

 void UDialogueDriver::TimerFired()
 {
	 if (charIndex < blocklen) {
		 Drive_Append_Char();
		 charIndex++;
	 }
	 else {
		 GetOwner()->GetWorldTimerManager().ClearTimer(DialogueScanTimer);
		 CurrentState = BlockFinished;
	 }

 }

 void UDialogueDriver::DialogueInteractReceived()
 {
	 switch (CurrentState)
	 {
	 case BlockFinished:
		 Next_Block_Implementation();
		 return;
	 case BlockInProgress:
		 Skip_Block_Implementation();
		 return;
	 }
 }

 void UDialogueDriver::StartDialogue()
 {
	 Dialogue_Window = NewObject<UDialogueWidget>(this, Window_Class);

	 if (Dialogue_Window) {
		 Dialogue_Window->AddToViewport(0);
		 Dialogue_Window->SetVisibility(ESlateVisibility::Visible);

		 if (GEngine) {
			 GetOwner()->EnableInput(GEngine->GetFirstLocalPlayerController(GetWorld()));
		 }

		 Next_Block_Name = First_Block_Name;
		 Next_Block_Implementation();
	 }
	 else {
		 Kill_Dialogue_Implementation();
	 }
 }

// Called when the game starts or when spawned
void UDialogueDriver::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->InputComponent = NewObject<UInputComponent>(GetOwner()); // initialize input component
    GetOwner()->InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &UDialogueDriver::DialogueInteractReceived);
	StartDialogue();
	//Dialogue window setup
	
}

// Called every frame
void UDialogueDriver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

