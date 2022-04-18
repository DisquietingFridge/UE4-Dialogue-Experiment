// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Misc/Char.h"
#include "Async/Async.h"
#include "Async/AsyncWork.h"

static uint8 NumCustomData = 2;




// Sets default values
AAutomataDriver::AAutomataDriver()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	// create arrays for pointers to reference
	TSharedPtr<TArray<bool>> currstates(new(TArray<bool>));
	TSharedPtr<TArray<bool>> nextstates(new(TArray<bool>));
	TSharedPtr<TArray<uint32>> neighbors(new(TArray<uint32>));

	CurrentStates = currstates;
	NextStates = nextstates;
	Neighbors = neighbors;
	
	
}

void AAutomataDriver::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	CellInstance = NewObject<UInstancedStaticMeshComponent>(this);
	CellInstance->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CellInstance->RegisterComponent();
	CellInstance->SetStaticMesh(Mesh);
	
	CellInstance->SetMobility(EComponentMobility::Static);

	DynMaterial = UMaterialInstanceDynamic::Create(Mat, this);
	CellInstance->SetMaterial(0, DynMaterial);

	DynMaterial->SetScalarParameterValue("PhaseExponent", PhaseExponent);
	DynMaterial->SetScalarParameterValue("EmissiveMultiplier", EmissiveMultiplier);
	DynMaterial->SetScalarParameterValue("Freq", 1 / StepPeriod);
	DynMaterial->SetScalarParameterValue("StepsToFade", StepsToFade);

	AddInstanceComponent(CellInstance);

	CellInstance->NumCustomDataFloats = NumCustomData;
}


// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	// create rulesets from input strings. 
	// e.g. FString Birth = "3", Survive = "23" for Conway's Game of Life
	for (TCHAR character : BirthString) 
	{
		if (TChar<TCHAR>::IsDigit(character)) 
		{
			BirthRules.Add(TChar<TCHAR>::ConvertCharDigitToInt(character));
		}
	}
	for (TCHAR character : SurviveString) 
	{
		if (TChar<TCHAR>::IsDigit(character)) 
		{
			SurviveRules.Add(TChar<TCHAR>::ConvertCharDigitToInt(character));
		}
	}

	//For the Next or Previous states, we need to store as many states as there are cells
	CurrentStates->Reserve(XDim * ZDim);
	NextStates->Reserve(XDim * ZDim);
	InitNeighborIndices();

	for (int32 CellID = 0; CellID < XDim * ZDim; ++CellID)
	{
		// derive grid coordinates from index
		int32 NewX = CellID % XDim;
		int32 NewZ = CellID / XDim;

		//Instance's transform is based on its grid coordinate
		CellInstance->AddInstance(FTransform((FVector(NewX, 0, NewZ) * Offset)));

		// Initialize state based on probability of being alive
		NextStates->Add((float(rand()) < (float(RAND_MAX) * Probability))); 

		// making previous state same as next state (as opposed to opposite) seems to be required. 
		// investigate why, but not super important 
		//(*CurrentStates).Add(!(*NextStates).Last());
		CurrentStates->Add(NextStates->Last());

		// define material data based on starting state
		int32 CellDataStart = NumCustomData * CellID;
		if ((*NextStates)[CellID]) 
		{ 
			CellInstance->PerInstanceSMCustomData[CellDataStart] = 0; // switched on at time 0
			CellInstance->PerInstanceSMCustomData[CellDataStart + 1] = -1; // switched off at arbitrary time before that
		}
		else 
		{
			CellInstance->PerInstanceSMCustomData[CellDataStart] = -(StepsToFade * StepPeriod) - 1;
			CellInstance->PerInstanceSMCustomData[CellDataStart + 1] = -StepsToFade*StepPeriod; // switched off long enough to have faded to 0
		}
	}

	CellInstance->InstanceUpdateCmdBuffer.NumEdits++;
	CellInstance->MarkRenderStateDirty();
	
	

	CellInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CellInstance->SetComponentTickEnabled(false);

	Processor = new FAsyncTask<CellProcessor>(this);
	Processor->StartBackgroundTask();
	
	// we are ready to start the iteration steps.
	GetWorldTimerManager().SetTimer(AutomataTimer, this, &AAutomataDriver::StepComplete, StepPeriod, true, 0);

}

void AAutomataDriver::StepComplete()
 {

	// have all the cells' next state calculated before sending to material
	Processor->EnsureCompletion();

	CellInstance->MarkRenderStateDirty();
	CellInstance->InstanceUpdateCmdBuffer.NumEdits++;
	

	//make previous state the next state
	ParallelFor(CurrentStates->Num(), [&](int32 CellID) 
	{
		(*CurrentStates)[CellID] = (*NextStates)[CellID];
	});

	// kick off calculation of next stage
	Processor->StartBackgroundTask();

}

// Define each cell's neighboring indices 
// (factoring in grid wraparound) to look up 
void AAutomataDriver::InitNeighborIndices()
{
	// A neighborhood is 8 cells, so number of
	// neighbors will be (number of cells) times 8
	Neighbors->Init(0, XDim * ZDim * 8);

	ParallelFor(XDim * ZDim, [&](int32 CellID) 
	{
		int32 zUp; // vertical coordinate above this cell
		int32 zDown; // vertical coordinate below this cell
		int32 xUp; // horizontal coordinate ahead of this cell
		int32 xDown; // horizontal coordinate behind this cell

		// derive grid coordinates from index
		int32 z = CellID / XDim;
		int32 x = CellID % XDim;

		// handle grid wrap-around in all four directions
		if (z + 1 == ZDim) 
		{
			zUp = 0;
		}
		else 
		{
			zUp = z + 1;
		}

		if (z == 0) 
		{
			zDown = ZDim - 1;
		}
		else 
		{
			zDown = z - 1;
		}

		if (x + 1 == XDim) 
		{
			xUp = 0;
		}
		else 
		{
			xUp = x + 1;
		}

		if (x == 0) 
		{
			xDown = XDim - 1;
		}
		else 
		{
			xDown = x - 1;
		}

		
		uint32 NeighborhoodStart = 8 * CellID;

		//assign lower neighborhood row IDs
		(*Neighbors)[NeighborhoodStart] = xDown + (XDim * zDown);
		(*Neighbors)[NeighborhoodStart + 1] = x + (XDim * zDown);
		(*Neighbors)[NeighborhoodStart + 2] = xUp + (XDim * zDown);

		//assign middle neighborhood row IDs
		(*Neighbors)[NeighborhoodStart + 3] = xDown + (XDim * z);
		(*Neighbors)[NeighborhoodStart + 4] = xUp + (XDim * z);

		// assign upper neighborhood row IDs
		(*Neighbors)[NeighborhoodStart + 5] = xDown + (XDim * zUp);
		(*Neighbors)[NeighborhoodStart + 6] = x + (XDim * zUp);
		(*Neighbors)[NeighborhoodStart + 7] = xUp + (XDim * zUp);
	});
}


CellProcessor::CellProcessor(AAutomataDriver* Driver)
	{
		this->Driver = Driver;

		XDim = Driver->GetXDim();
		ZDim = Driver->GetZDim();

		BirthRules = Driver->GetBirthRules();
		SurviveRules = Driver->GetSurviveRules();

		CellInstance = Driver->GetCellInstance();

		CurrentStates = Driver->GetCurrentStates();
		NextStates = Driver->GetNextStates();
		Neighbors = Driver->GetNeighbors();
	}

	void CellProcessor::DoWork()
	{

		float time = Driver->GetWorld()->GetTimeSeconds();
		ParallelFor(CurrentStates->Num(), [&](int32 CellID) 
		{
			// Query the cell's neighborhood to sum its alive neighbors
			uint8 AliveNeighbors = 0;
			uint32 NeighborhoodStart = 8 * CellID;
			for (uint8 NeighborID = 0; NeighborID < 8; ++NeighborID)
			{
				int32 CandidateCellID = Neighbors->operator[](NeighborhoodStart + NeighborID);
				AliveNeighbors += int32(CurrentStates->operator[](CandidateCellID));
			}
		
			// Apply automata rules
			if ((*CurrentStates)[CellID]) 
			{
				(*NextStates)[CellID] =  SurviveRules->Contains(AliveNeighbors);  // Any live cell with appropriate amount of neighbors survives
			}
			else 
			{
				(*NextStates)[CellID] = BirthRules->Contains(AliveNeighbors); //Any dead cell with appropriate amount of neighbors becomes alive
			}

			if ((*NextStates)[CellID] != (*CurrentStates)[CellID]) // a change has occurred
			{ // register change based on state
				int32 CellDataStart = NumCustomData * CellID;
				if ((*NextStates)[CellID]) 
				{ 
					CellInstance->PerInstanceSMCustomData[CellDataStart] = time;
				}
				else 
				{
					CellInstance->PerInstanceSMCustomData[CellDataStart + 1] = time;
				}
			}
		}
		
		);
	}
