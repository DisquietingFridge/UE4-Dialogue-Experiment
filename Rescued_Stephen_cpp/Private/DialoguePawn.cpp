// Fill out your copyright notice in the Description page of Project Settings.


#include "DialoguePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Internationalization/Text.h"
#include "GameFramework/Controller.h"

// Sets default values
ADialoguePawn::ADialoguePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;


	// effectively initialized as a usable "default" case
	Repossess_Target = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
	Controller_Rotation = Repossess_Target->GetControlRotation();
	
	// these are not usable in their default initialization. Have to be initialized afterwards.
	Dialogue_Window = nullptr;
	Lines_Table = nullptr;
	Line = "";
	Nametag = FText::FromString("");

	
}

// Called when the game starts or when spawned
void ADialoguePawn::BeginPlay()
{
	Super::BeginPlay();
	
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

