// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"
#include "Materials/MaterialParameterCollectionInstance.h"

// Sets default values
AAutomataDriver::AAutomataDriver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Root Component"));

}

void AAutomataDriver::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	Cell_Instance = NewObject<UInstancedStaticMeshComponent>(this);
	Cell_Instance->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Cell_Instance->RegisterComponent();
	Cell_Instance->SetStaticMesh(Mesh);
	Cell_Instance->SetMaterial(0, Mat);
	AddInstanceComponent(Cell_Instance);

	collection = GetWorld()->GetParameterCollectionInstance(params);
	collection->SetScalarParameterValue(TEXT("fractionComplete"), 1);


	Cell_Instance->NumCustomDataFloats = 2;

}


// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();



	FVector BaseTranslation = GetTransform().GetLocation();


	for (int32 i = 0; i < Xdim * Zdim; i++)
	{
		int32 newX = i % Xdim;
		int32 newZ = i / Zdim;

		Cell_Instance->AddInstance(FTransform(BaseTranslation + (FVector(newX, 0, newZ) * offset)));
		

		Next_States.Add((float(rand()) / float(RAND_MAX)) < P); // Initialize next state based on cell probability

		//debuggy test line dw about it
		Previous_States.Add(!Next_States.Last());

		//Cell_Instance->SetCustomDataValue(i, 0, Previous_States[i], false);
		//Cell_Instance->SetCustomDataValue(i, 1, Next_States[i], false);
	}

	GetWorldTimerManager().SetTimer(AutomataTimer, this, &AAutomataDriver::StepComplete, period, true, 0);

}



void AAutomataDriver::StepComplete()
{
	collection->SetScalarParameterValue(TEXT("fractionComplete"), 1);
	SetActorTickEnabled(false);


	// reset time cycle
	theta = 0;
	collection->SetScalarParameterValue(TEXT("fractionComplete"), theta);

	for (int i = 0; i < Next_States.Num(); i++)
	{
		bool temp = Previous_States[i];
		Previous_States[i] = Next_States[i];
		Next_States[i] = temp;

		Cell_Instance->SetCustomDataValue(i, 0, Previous_States[i], true);
		Cell_Instance->SetCustomDataValue(i, 1, Next_States[i], true);

	}
}

// Called every frame
void AAutomataDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float deltaFrac = DeltaTime / period;
	theta = theta + deltaFrac;
	collection->SetScalarParameterValue(TEXT("fractionComplete"), theta);
	

}

