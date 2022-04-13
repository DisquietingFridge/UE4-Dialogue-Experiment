// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"
#include "Materials/MaterialParameterCollectionInstance.h"

// Sets default values
AAutomataDriver::AAutomataDriver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	freq = float(1 / period);

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



	for (int32 i = 0; i < Xdim * Zdim; i++)
	{
		int32 newX = i % Xdim;
		int32 newZ = i / Xdim;

		Cell_Instance->AddInstance(FTransform((FVector(newX, 0, newZ) * offset)));


		Next_States.Add((float(rand()) / float(RAND_MAX)) < P); // Initialize next state based on cell probability

		
		Previous_States.Add(1 - Next_States.Last());
	}

	GetWorldTimerManager().SetTimer(AutomataTimer, this, &AAutomataDriver::StepComplete, period, true, 0);

}



void AAutomataDriver::StepComplete()
{

	int zUp;
	int zDown;
	int xUp;
	int xDown;

	// reset time cycle
	collection->SetScalarParameterValue(TEXT("fractionComplete"), theta = 0);

	// every tile- set this tile's PrevState to equal this tile's NextState

	for (int i = 0; i < Previous_States.Num(); i++) {
		Previous_States[i] = Next_States[i];
	}

	// every tile- collect neighbors' PrevStates, assign this tile's new NextState
			//set this tile's custom instance data from PrevStates and NewStates
	for (int z = 0; z < Zdim; z++) {
		
		if (z + 1 == Zdim) {
			zUp = 0;
		}
		else {
			zUp = z + 1;
		}

		if (z == 0) {
			zDown = Zdim - 1;
		} 
		else {
			zDown = z - 1;
		}

		for (int x = 0; x < Xdim; x++) {

			if (x + 1 == Xdim) {
				xUp = 0;
			}
			else {
				xUp = x + 1;
			}

			if (x == 0) {
				xDown = Xdim - 1;
			}
			else {
				xDown = x - 1;
			}

			int aliveNeighbors =
				//lower row
				Previous_States[xDown + (Zdim * zDown)] +
				Previous_States[x + (Zdim * zDown)] +
				Previous_States[xUp + (Zdim * zDown)] +

				//same row
				Previous_States[xDown + (Zdim * z)] +
				Previous_States[xUp + (Zdim * z)] +

				//upper row
				Previous_States[xDown + (Zdim * zUp)] +
				Previous_States[x + (Zdim * zUp)] +
				Previous_States[xUp + (Zdim * zUp)];

			int i = x + (Zdim * z);
			
			if (Previous_States[i] == 1) { // alive cell
				if ((aliveNeighbors == 2) || (aliveNeighbors == 3)) { // Any live cell with two or three live neighbours survives.
					Next_States[i] = 1;
				}
				else { // otherwise live cell dies
					Next_States[i] = 0;
				}
			}
			else { // dead cell
				if (aliveNeighbors == 3) {
					Next_States[i] = 1; // Any dead cell with three live neighbours becomes a live cell.
				}
				else {
					Next_States[i] = 0; // otherwise stays dead
				}
			}
		
			// update cell material
			Cell_Instance->SetCustomDataValue(i, 0, Previous_States[i], true);
			Cell_Instance->SetCustomDataValue(i, 1, Next_States[i], true);

		}
	}
}

// Called every frame
void AAutomataDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	theta = theta + (DeltaTime * freq);

	collection->SetScalarParameterValue(TEXT("fractionComplete"), theta);
	

}

