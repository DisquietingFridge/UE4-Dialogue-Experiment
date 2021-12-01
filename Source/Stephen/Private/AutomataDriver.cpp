// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"

// Sets default values
AAutomataDriver::AAutomataDriver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Rooter = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Root Test"));
	RootComponent = Rooter;

}

// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	if (Cell_Type) {
		Cell = NewObject<UAutomataCell>(this, Cell_Type, TEXT("Son"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, (Cell->GetName() + " created"));
		Cell->RegisterComponent();
		//this->SetRootComponent(Cell);
		Cell->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	}

	
}

// Called every frame
void AAutomataDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("Driver created"));

}

