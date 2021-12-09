// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"

// Sets default values
AAutomataDriver::AAutomataDriver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Root Test"));

}

// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	if (Cell_Type) {
		int32 len = Xdim * Ydim;
		for (int32 i = 0; i < len; i++)
		{
			Cell_Array.Add(NewObject<UAutomataCell>(this, Cell_Type));
			Cell_Array[i]->RegisterComponent();
			Cell_Array[i]->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			Cell_Array[i]->SetRelativeTransform(FTransform(FVector((i%Xdim)*offset, (i/Ydim)*offset, 0)));

		}
	}

	
}

// Called every frame
void AAutomataDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

