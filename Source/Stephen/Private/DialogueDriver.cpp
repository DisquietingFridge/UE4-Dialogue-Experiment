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

void UDialogueDriver::StartNewBlock_Implementation(FName NextBlockName)
 {

	 //import and process table row
	 FDialogueData* imported = Lines_Table->FindRow<FDialogueData>(NextBlockName, "");
	 
	 if (imported)
	 {
		 CurrentData = *imported;
	 }
	 else
	 {
		 EndDialogue_Implementation();
		 return;
	 }

	 //initialize things for new block
	 Dialogue_Window->HideAllChars();
	 Dialogue_Window->SetBlockData(CurrentData.BlockText);

	 //start timer
	 GetOwner()->GetWorldTimerManager().SetTimer(DialogueScanTimer, this, &UDialogueDriver::TimerFired, Scan_Period, true, Scan_Period);

	 CurrentState = BlockInProgress;
 }

 void UDialogueDriver::FinishBlock_Implementation()
 {
	 GetOwner()->GetWorldTimerManager().ClearTimer(DialogueScanTimer);

	 Dialogue_Window->RevealAllChars();

	 CurrentState = BlockFinished;
 }

 void UDialogueDriver::EndDialogue_Implementation() 
 {
	 if (Dialogue_Window) {
		 Dialogue_Window->RemoveFromViewport();
		 Dialogue_Window = nullptr; // remove for garbage collection
	 }

	 GetOwner()->DisableInput(GEngine->GetFirstLocalPlayerController(GetWorld()));
 }

 void UDialogueDriver::TimerFired()
 {
	 if (Dialogue_Window->RevealCharsAndIsDone())
	 {
		 GetOwner()->GetWorldTimerManager().ClearTimer(DialogueScanTimer);
		 CurrentState = BlockFinished;
	 }
 }

 void UDialogueDriver::DialogueInteractReceived()
 {
	 switch (CurrentState)
	 {
	 case BlockFinished:
		 StartNewBlock_Implementation(CurrentData.NextBlockName);
		 return;
	 case BlockInProgress:
		 FinishBlock_Implementation();
		 return;
	 }
 }

 void UDialogueDriver::StartDialogue()
 {
	 Dialogue_Window = NewObject<UDialogueWidget>(this, Window_Class);

	 if (Dialogue_Window) {
		 Dialogue_Window->AddToViewport(0);
		 Dialogue_Window->SetVisibility(ESlateVisibility::Visible);

		 StartNewBlock_Implementation(First_Block_Name);
	 }
	 else {
		 EndDialogue_Implementation();
	 }
 }

// Called when the game starts or when spawned
void UDialogueDriver::BeginPlay()
{
	Super::BeginPlay();
	SetupInput();
	StartDialogue();
	
}

void UDialogueDriver::SetupInput()
{
	GetOwner()->InputComponent = NewObject<UInputComponent>(GetOwner()); // initialize input component
	GetOwner()->InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &UDialogueDriver::DialogueInteractReceived);
	if (GEngine) 
	{
		GetOwner()->EnableInput(GEngine->GetFirstLocalPlayerController(GetWorld()));
	}

}

// Called every frame
void UDialogueDriver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

