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

	DynMaterial = UMaterialInstanceDynamic::Create(Mat, this);
	Cell_Instance->SetMaterial(0, DynMaterial);
	DynMaterial->SetScalarParameterValue("FractionComplete", 0);

	AddInstanceComponent(Cell_Instance);

	Cell_Instance->NumCustomDataFloats = 2;

}


// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	Next_States.Reserve(Xdim * Zdim);
	Previous_States.Reserve(Xdim * Zdim);

	for (int32 i = 0; i < Xdim * Zdim; i++)
	{
		int32 newX = i % Xdim;
		int32 newZ = i / Xdim;

		Cell_Instance->AddInstance(FTransform((FVector(newX, 0, newZ) * offset)));


		Next_States.Add((float(rand()) / float(RAND_MAX)) < P); // Initialize next state based on cell probability

		Previous_States.Add(!Next_States.Last());
	}

	GetWorldTimerManager().SetTimer(AutomataTimer, this, &AAutomataDriver::StepComplete, period, true, 0);

}



void AAutomataDriver::StepComplete()
{
	// reset time cycle
	DynMaterial->SetScalarParameterValue(TEXT("fractionComplete"), theta = 0);

	// every tile- set this tile's PrevState to equal this tile's NextState
	ParallelFor(Previous_States.Num(), [&](int32 i) {
		Previous_States[i] = Next_States[i];
	});
	

	// every tile- collect neighbors' PrevStates, assign this tile's new NextState
			//set this tile's custom instance data from PrevStates and NewStates
	FCriticalSection Mutex;
	ParallelFor(Previous_States.Num(), [&](int32 i) {

		int32 zUp;
		int32 zDown;
		int32 xUp;
		int32 xDown;

		int32 z = i / Xdim;
		int32 x = i % Xdim;


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

		uint8 aliveNeighbors =
			//lower row
			uint8(Previous_States[xDown + (Zdim * zDown)]) +
			uint8(Previous_States[x + (Zdim * zDown)]) +
			uint8(Previous_States[xUp + (Zdim * zDown)]) +

			//same row
			uint8(Previous_States[xDown + (Zdim * z)]) +
			uint8(Previous_States[xUp + (Zdim * z)]) +

			//upper row
			uint8(Previous_States[xDown + (Zdim * zUp)]) +
			uint8(Previous_States[x + (Zdim * zUp)]) +
			uint8(Previous_States[xUp + (Zdim * zUp)]);

		if (Previous_States[i] == true) { // alive cell
			if (Survive.Contains(aliveNeighbors)) { // Any live cell with appropriate amount of neighbors survives
				Next_States[i] = true;
			}
			else { // otherwise live cell dies
				Next_States[i] = false;
			}
		}
		else { // dead cell
			if (Birth.Contains(aliveNeighbors)) { //Any dead cell with  appropriate amount of neighbors becomes alive
				Next_States[i] = true;
			}
			else {
				Next_States[i] = false; // otherwise stays dead
			}
		}

		// update cell material
		Mutex.Lock();
		Cell_Instance->SetCustomDataValue(i, 0, Previous_States[i], true);
		Mutex.Unlock();

		if (Next_States[i] != Previous_States[i]) {

			Mutex.Lock();
			Cell_Instance->SetCustomDataValue(i, 1, Next_States[i], true);
			Mutex.Unlock();
		}
	});

}

// Called every frame
void AAutomataDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DynMaterial->SetScalarParameterValue(TEXT("FractionComplete"), theta = theta + (DeltaTime * freq));
	
}

