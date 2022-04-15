// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Misc/Char.h"

// Sets default values
AAutomataDriver::AAutomataDriver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	

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
	DynMaterial->SetScalarParameterValue("PhaseExponent", phaseExponent);
	DynMaterial->SetScalarParameterValue("EmissiveMultiplier", emissiveMultiplier);
	DynMaterial->SetScalarParameterValue("Freq", float(1 / period));
	DynMaterial->SetScalarParameterValue("StepsToFade", stepsToFade);


	AddInstanceComponent(Cell_Instance);

	Cell_Instance->NumCustomDataFloats = 2;

}


// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	for (TCHAR character : Birth) {
		if (TChar<TCHAR>::IsDigit(character)) {
			BirthRules.Add(TChar<TCHAR>::ConvertCharDigitToInt(character));
		}
	}

	for (TCHAR character : Survive) {
		if (TChar<TCHAR>::IsDigit(character)) {
			SurviveRules.Add(TChar<TCHAR>::ConvertCharDigitToInt(character));
		}
	}

	Next_States.Reserve(Xdim * Zdim);
	Previous_States.Reserve(Xdim * Zdim);

	for (int32 i = 0; i < Xdim * Zdim; i++)
	{
		int32 newX = i % Xdim;
		int32 newZ = i / Xdim;

		Cell_Instance->AddInstance(FTransform((FVector(newX, 0, newZ) * offset)));


		Next_States.Add((float(rand()) / float(RAND_MAX)) < P); // Initialize next state based on cell probability

		Previous_States.Add(!Next_States.Last());

		if (Next_States[i]) { // if starting on

			Cell_Instance->PerInstanceSMCustomData[2 * i] = 0; // switched on at time 0
			Cell_Instance->PerInstanceSMCustomData[2 * i + 1] = -1; // switched off at arbitrary time before that

		}
		else { // starting off

			Cell_Instance->PerInstanceSMCustomData[2 * i] = -(stepsToFade * period) - 1;
			Cell_Instance->PerInstanceSMCustomData[2 * i + 1] = -stepsToFade*period; // switched off long enough to have faded to 0

		}


	}Cell_Instance->InstanceUpdateCmdBuffer.NumEdits++;
	

	GetWorldTimerManager().SetTimer(AutomataTimer, this, &AAutomataDriver::StepComplete, period, true, 0);

}



void AAutomataDriver::StepComplete()
 {
	// reset time cycle
	//DynMaterial->SetScalarParameterValue("FractionComplete", theta = 0);
	float time = GetWorld()->GetTimeSeconds();
	//DynMaterial->SetScalarParameterValue("PrevTime", GetWorld()->GetTimeSeconds());
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
			uint8(Previous_States[xDown + (Xdim * zDown)]) +
			uint8(Previous_States[x + (Xdim * zDown)]) +
			uint8(Previous_States[xUp + (Xdim * zDown)]) +

			//same row
			uint8(Previous_States[xDown + (Xdim * z)]) +
			uint8(Previous_States[xUp + (Xdim * z)]) +

			//upper row
			uint8(Previous_States[xDown + (Xdim * zUp)]) +
			uint8(Previous_States[x + (Xdim * zUp)]) +
			uint8(Previous_States[xUp + (Xdim * zUp)]);

		if (Previous_States[i] == true) { // alive cell
			Next_States[i] = SurviveRules.Contains(aliveNeighbors);  // Any live cell with appropriate amount of neighbors survives
		}
		else { // dead cell
			Next_States[i] = BirthRules.Contains(aliveNeighbors); //Any dead cell with  appropriate amount of neighbors becomes alive
		}

		// update cell material
		if (Next_States[i] != Previous_States[i]) { // a change has occurred
			if (Next_States[i] == true) {
				Cell_Instance->PerInstanceSMCustomData[i * 2] = time; // register switched-on time
			}
			else {
				Cell_Instance->PerInstanceSMCustomData[i * 2 + 1] = time; // register switched-off time
			}
		}


		/*
		Cell_Instance->PerInstanceSMCustomData[i * 2] = float(Previous_States[i]);
		Cell_Instance->PerInstanceSMCustomData[i*2 + 1] = float(Next_States[i]);
		*/
	

		
		
		
	});
	Cell_Instance->MarkRenderStateDirty();
	Cell_Instance->InstanceUpdateCmdBuffer.NumEdits++;

	

}

// Called every frame
void AAutomataDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DynMaterial->SetScalarParameterValue(TEXT("FractionComplete"), theta = theta + (DeltaTime * freq));
	
}

