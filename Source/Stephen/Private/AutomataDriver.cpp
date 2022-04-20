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
	
}

void AAutomataDriver::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	DynMaterial = UMaterialInstanceDynamic::Create(Mat, this);
	DynMaterial->SetScalarParameterValue("PhaseExponent", PhaseExponent);
	DynMaterial->SetScalarParameterValue("EmissiveMultiplier", EmissiveMultiplier);
	DynMaterial->SetScalarParameterValue("FadePerSecond", 1 / (StepPeriod*StepsToFade));
	DynMaterial->SetScalarParameterValue("StepTransitionTime", StepPeriod);

	CellInstance = NewObject<UInstancedStaticMeshComponent>(this);
	CellInstance->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CellInstance->RegisterComponent();
	CellInstance->SetStaticMesh(Mesh);
	CellInstance->SetMobility(EComponentMobility::Static);
	CellInstance->SetMaterial(0, DynMaterial);
	AddInstanceComponent(CellInstance);
	CellInstance->NumCustomDataFloats = NumCustomData;
}

void AAutomataDriver::PostInitializeComponents()
{

	Super::PostInitializeComponents();

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

	TSharedPtr<TArray<bool>> currstates(new(TArray<bool>));
	CurrentStates = currstates;
	CurrentStates->Init(0, XDim * ZDim);

	TArray<FTransform> transforms;
	transforms.Init(FTransform(), XDim * ZDim);

	// calculate transforms and current-state for each cell
	ParallelFor(XDim * ZDim, [&](int32 CellID)
	{
		// derive grid coordinates from index
		int32 NewX = CellID % XDim;
		int32 NewZ = CellID / XDim;

		//Instance's transform is based on its grid coordinate
		transforms[CellID] = FTransform((FVector(NewX, 0, NewZ) * Offset));

		// Initialize state based on probability of being alive
		(*CurrentStates)[CellID] = (float(rand()) < (float(RAND_MAX) * Probability));
	});

	CellInstance->AddInstances(transforms, false);
	CellInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CellInstance->SetComponentTickEnabled(false);

	ParallelFor(XDim * ZDim, [&](int32 CellID)
	{ // set starting state in the "Next" slot (will be shifted to "Current" when true Next is calculated)
		if ((*CurrentStates)[CellID])
		{// set switch-off time to the future
			CellInstance->PerInstanceSMCustomData[NumCustomData * CellID + 1] = StepPeriod * 2; 
		}
		else
		{ // set switch-off time far enough in the past to have faded completely
			CellInstance->PerInstanceSMCustomData[NumCustomData * CellID + 1] = -StepPeriod * StepsToFade; 
		}
 
	});

	// neighborhood setup
	TSharedPtr<TArray<uint32>> neighbors(new(TArray<uint32>));
	Neighbors = neighbors;
	InitNeighborIndices();

	// time to calculate nextsteps
	TSharedPtr<TArray<bool>> nextstates(new(TArray<bool>));
	NextStates = nextstates;
	NextStates->Init(0, XDim * ZDim);
	Processor = new FAsyncTask<CellProcessor>(this);
	Time = GetWorld()->GetTimeSeconds();
	Processor->StartSynchronousTask();

	CellInstance->InstanceUpdateCmdBuffer.NumEdits++;
	CellInstance->MarkRenderStateDirty();
}


// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	//Time = GetWorld()->GetTimeSeconds();
	
	// we are ready to start the iteration steps.
	GetWorldTimerManager().SetTimer(StepTimer, this, &AAutomataDriver::StepComplete, StepPeriod, true, 0);
	GetWorldTimerManager().SetTimer(InstanceUpdateTimer, this, &AAutomataDriver::UpdateInstance, StepPeriod, true, StepPeriod*(1+PeriodOffset));
}

void AAutomataDriver::StepComplete()
 {

	// have all the cells' next state calculated before sending to material
	Time = GetWorld()->GetTimeSeconds();

	Processor->EnsureCompletion();
	
	// make new Current state the old Next state.

	ParallelFor((*CurrentStates).Num(), [&](int32 i) {
		(*CurrentStates)[i] = (*NextStates)[i];
	});
	
	// kick off calculation of next stage
	Processor->StartBackgroundTask();

}

void AAutomataDriver::UpdateInstance()
{
	Processor->EnsureCompletion();

	CellInstance->MarkRenderStateDirty();
	CellInstance->InstanceUpdateCmdBuffer.NumEdits++;
	DynMaterial->SetScalarParameterValue("StepTransitionTime", Time + StepPeriod);

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

		StepPeriod = Driver->GetStepPeriod();
	}

	void CellProcessor::DoWork()
	{

		float NextStepTime = Driver->GetTime() + StepPeriod;
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

			int32 NextDataID = NumCustomData * CellID + 1;

			//shift material state in time
			CellInstance->PerInstanceSMCustomData[NextDataID - 1] = CellInstance->PerInstanceSMCustomData[NextDataID];

			if ((*NextStates)[CellID])
			{ // register change based on state
				CellInstance->PerInstanceSMCustomData[NextDataID] = NextStepTime + 3*StepPeriod; // switch-off time is in the future, i.e. cell is still on
			}
			else // is off at next time
			{
				if ((*CurrentStates)[CellID])  // was previously on
				{ // register switch-off time as being upcoming step
					CellInstance->PerInstanceSMCustomData[NextDataID] = NextStepTime;
				}
				else // preserve old switch-off time
				{
					CellInstance->PerInstanceSMCustomData[NextDataID] = CellInstance->PerInstanceSMCustomData[NextDataID - 1];
				}
				
			}
		}
		
		);
	}
