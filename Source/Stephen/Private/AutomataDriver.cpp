// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomataDriver.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Misc/Char.h"
#include "Async/Async.h"
#include "Async/AsyncWork.h"
#include "Containers/ArrayView.h"

static uint8 NumCustomData = 9;

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
	DynMaterial->SetScalarParameterValue("FadePerSecond", 1 / (StepPeriod * StepsToFade));
	//DynMaterial->SetScalarParameterValue("StepTransitionTime", StepPeriod);


	//Set up InstanceComponents
	ClusterInstances.Reserve(Divisions);
	for (int i = 0; i < Divisions; ++i)
	{
		UInstancedStaticMeshComponent* NewClusterInstance = NewObject<UInstancedStaticMeshComponent>(this);

		NewClusterInstance->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewClusterInstance->RegisterComponent();
		NewClusterInstance->SetStaticMesh(Mesh);
		NewClusterInstance->SetMobility(EComponentMobility::Static);
		NewClusterInstance->SetMaterial(0, DynMaterial);
		AddInstanceComponent(NewClusterInstance);
		NewClusterInstance->NumCustomDataFloats = NumCustomData;

		ClusterInstances.Add(NewClusterInstance);
	}
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

	uint32 NumClusters = XDim * ZDim;
	uint32 NumCells = 4 * NumClusters;

	// initialize CurrentStates
	TSharedPtr<TArray<bool>> currstates(new(TArray<bool>));
	CurrentStates = currstates;
	CurrentStates->Init(0, NumCells);

	// Initialize Transforms
	TArray<FTransform> Transforms;
	Transforms.Init(FTransform(), NumClusters);

	// calculate transforms for each cluster
	ParallelFor(NumClusters, [&](int32 ClusterID)
	{
		// derive grid coordinates from index
		int32 ClusterX = ClusterID % XDim;
		int32 ClusterZ = ClusterID / XDim;

		//Instance's transform is based on its grid coordinate
		Transforms[ClusterID] = FTransform((FVector(ClusterX, 0, ClusterZ) * Offset));
	});

	// calculate state for each cell
	ParallelFor(NumCells, [&](int32 CellID)
	{
		// Initialize state based on probability of being alive

		(*CurrentStates)[CellID] = (float(rand()) < (float(RAND_MAX) * Probability));
	});

	// the last ClusterInstance component may have fewer cells assigned to it
	int MaxClustersPerInstance = int(ceilf(float(NumClusters) / float(Divisions)));
	// Add instances to each ClusterInstance component, applying appropriate transform
	for (uint32 ClusterID = 0; ClusterID < NumClusters; ++ClusterID)
	{
		int InstanceIndex = ClusterID / MaxClustersPerInstance;
		ClusterInstances[InstanceIndex]->AddInstance(Transforms[ClusterID]);
	}
	
	// make sure all instance components have no collision and no ticking
	for (UInstancedStaticMeshComponent* ClusterInstance : ClusterInstances)
	{
		ClusterInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ClusterInstance->SetComponentTickEnabled(false);
	}
	

	// set starting state in the "Next" slot
	// (will be set to "Current" slot when actual Next is calculated)
	ParallelFor(NumClusters, [&](int32 ClusterID)
	{
		int InstanceIndex = ClusterID / MaxClustersPerInstance;
		int InstanceClusterID = ClusterID - InstanceIndex * MaxClustersPerInstance;

		int32 ClusterX = ClusterID % XDim;
		int32 ClusterZ = ClusterID / XDim;

		//CellIDs contained within this cluster
		TArray<uint32> CellIDs
		{
			(2 * ClusterZ * 2 * XDim) + (2 * ClusterX), // bottom left
			(2 * ClusterZ * 2 * XDim) + (2 * ClusterX + 1), // bottom right
			((2 * ClusterZ + 1) * 2 * XDim) + (2 * ClusterX), // upper left
			((2 * ClusterZ + 1) * 2 * XDim) + (2 * ClusterX + 1), // upper right
		};

		for (uint8 Quadrant = 0; Quadrant < 4; ++Quadrant)
		{

			uint8 DataIndex = NumCustomData * InstanceClusterID + (2 * Quadrant) + 1;

			if ((*CurrentStates)[CellIDs[Quadrant]])
			{// set switch-off time to the far future
				ClusterInstances[InstanceIndex]->PerInstanceSMCustomData[DataIndex] = StepPeriod * 2;
			}
			else
			{ // set switch-off time far enough in the past to have faded completely
				ClusterInstances[InstanceIndex]->PerInstanceSMCustomData[DataIndex] = -2 * (StepPeriod * StepsToFade);
			}

		}
		
	});

	// neighborhood setup
	TSharedPtr<TArray<uint32>> neighbors(new(TArray<uint32>));
	Neighbors = neighbors;
	InitNeighborIndices();

	// Initializing Nextsteps
	TSharedPtr<TArray<bool>> nextstates(new(TArray<bool>));
	NextStates = nextstates;
	NextStates->Init(0, NumCells);

	// create processors
	Processors.Reserve(Divisions);
	for (int i = 0; i < Divisions; ++i)
	{
		uint32 StartingIndex = i * MaxClustersPerInstance;
		FAsyncTask<CellProcessor>* NewProcessor = new FAsyncTask<CellProcessor>(this, ClusterInstances[i], StartingIndex);
		Processors.Add(NewProcessor);
	}

	Time = GetWorld()->GetTimeSeconds();

	// start the processes (to calculate Next Step for all the cells)
	CurrentProcess = 0;
	Processors[CurrentProcess]->StartSynchronousTask();
	// having kicked off the first process, they should all cascade to completion, until the final one is complete
	// wait until the last process is complete, but don't start it by itself.
	Processors[Divisions - 1]->EnsureCompletion(false);


	// mark everything as dirty (do we need to do this as part of initialization?)
	for (UInstancedStaticMeshComponent* ClusterInstance : ClusterInstances)
	{
		ClusterInstance->InstanceUpdateCmdBuffer.NumEdits++;
		ClusterInstance->MarkRenderStateDirty();
	}
	
}


// Called when the game starts or when spawned
void AAutomataDriver::BeginPlay()
{
	Super::BeginPlay();

	//Though we should be currently at time 0, we put time a bit ahead
	// Since we're skipping a period before starting our timers/processors
	Time = StepPeriod;
	
	// we are ready to start the iteration steps.
	float TimeFraction = 1 / float (Divisions + 1);
	GetWorldTimerManager().SetTimer(StepTimer, this, &AAutomataDriver::TimerFired, StepPeriod * TimeFraction, true, StepPeriod);
	//GetWorldTimerManager().SetTimer(InstanceUpdateTimer, this, &AAutomataDriver::UpdateInstance, StepPeriod, true, StepPeriod*(1+PeriodOffset));
}

void AAutomataDriver::TimerFired()
{
	if (MaterialToUpdate != Divisions)
	{
		UpdateInstance(MaterialToUpdate);
		MaterialToUpdate++;
		
	}
	else
	{
		StepComplete();
	}
}

void AAutomataDriver::StepComplete()
 {

	// have all the cells' next state calculated before sending to material
	// strictly speaking we only need to check the last one, but
	// checking all for safety
	for (FAsyncTask<CellProcessor>* Process : Processors)
	{
		Process->EnsureCompletion();
	}
	
	Time = GetWorld()->GetTimeSeconds();

	// make new Current state the old Next state.

	ParallelFor((*CurrentStates).Num(), [&](int32 i) 
	{
		(*CurrentStates)[i] = (*NextStates)[i];
	});
	
	// kick off calculation of next stage
	CurrentProcess = 0;
	MaterialToUpdate = 0;
	Processors[0]->EnsureCompletion();
	Processors[0]->StartBackgroundTask();

}

void AAutomataDriver::UpdateInstance(uint32 Index)
{
	// Safety check
	//Processors[Index]->EnsureCompletion();

	ClusterInstances[Index]->MarkRenderStateDirty();
	ClusterInstances[Index]->InstanceUpdateCmdBuffer.NumEdits++;
	//DynMaterials[Index]->SetScalarParameterValue("StepTransitionTime", Time + StepPeriod);

}

void AAutomataDriver::ProcessCompleted()
{
	CurrentProcess = (CurrentProcess + 1) % Divisions;

	if (CurrentProcess != 0)
	{
		Processors[CurrentProcess]->EnsureCompletion();
		Processors[CurrentProcess]->StartBackgroundTask();
	}
}

// Define each cell's neighboring indices 
// (factoring in grid wraparound) to look up 
void AAutomataDriver::InitNeighborIndices()
{
	
	int NumCells = XDim * ZDim * 4;
	// A neighborhood is 8 cells, so number of
	// neighbors will be (number of cells) times 8
	Neighbors->Init(0, NumCells * 8);

	ParallelFor(NumCells, [&](int32 CellID) 
	{
		int32 zUp; // vertical coordinate above this cell
		int32 zDown; // vertical coordinate below this cell
		int32 xUp; // horizontal coordinate ahead of this cell
		int32 xDown; // horizontal coordinate behind this cell

		int XCells = XDim * 2;
		int ZCells = ZDim * 2;

		// derive grid coordinates from index
		int32 z = CellID / XCells;
		int32 x = CellID % XCells;

		// handle grid wrap-around in all four directions
		if (z + 1 == ZCells) 
		{
			zUp = 0;
		}
		else 
		{
			zUp = z + 1;
		}

		if (z == 0) 
		{
			zDown = ZCells - 1;
		}
		else 
		{
			zDown = z - 1;
		}

		if (x + 1 == XCells) 
		{
			xUp = 0;
		}
		else 
		{
			xUp = x + 1;
		}

		if (x == 0) 
		{
			xDown = XCells - 1;
		}
		else 
		{
			xDown = x - 1;
		}

		
		uint32 NeighborhoodStart = 8 * CellID;

		//assign lower neighborhood row IDs
		(*Neighbors)[NeighborhoodStart] = xDown + (XCells * zDown);
		(*Neighbors)[NeighborhoodStart + 1] = x + (XCells * zDown);
		(*Neighbors)[NeighborhoodStart + 2] = xUp + (XCells * zDown);

		//assign middle neighborhood row IDs
		(*Neighbors)[NeighborhoodStart + 3] = xDown + (XCells * z);
		(*Neighbors)[NeighborhoodStart + 4] = xUp + (XCells * z);

		// assign upper neighborhood row IDs
		(*Neighbors)[NeighborhoodStart + 5] = xDown + (XCells * zUp);
		(*Neighbors)[NeighborhoodStart + 6] = x + (XCells * zUp);
		(*Neighbors)[NeighborhoodStart + 7] = xUp + (XCells * zUp);
	});
}


CellProcessor::CellProcessor(AAutomataDriver* Driver, UInstancedStaticMeshComponent* ClusterInstance, uint32 StartingIndex)
	{
		this->Driver = Driver;
		this->ClusterInstance = ClusterInstance;
		this->StartingIndex = StartingIndex;
		
		NumElements = ClusterInstance->GetInstanceCount();

		BirthRules = Driver->GetBirthRules();
		SurviveRules = Driver->GetSurviveRules();

		CurrentStates = Driver->GetCurrentStates();
		NextStates = Driver->GetNextStates();
		Neighbors = Driver->GetNeighbors();

		StepPeriod = Driver->GetStepPeriod();

		XDim = Driver->GetXDim();
	}

	void CellProcessor::DoWork()
	{
		float NextStepTime = Driver->GetTime() + StepPeriod;
		ParallelFor(NumElements, [&](int32 ProcessorCluster) 
		{
			int ClusterID = StartingIndex + ProcessorCluster;

			int32 ClusterX = ClusterID % XDim;
			int32 ClusterZ = ClusterID / XDim;

			TArray<uint32> CellIDs
			{
				(2 * ClusterZ * 2 * XDim) + (2 * ClusterX), // bottom left
				(2 * ClusterZ * 2 * XDim) + (2 * ClusterX + 1), // bottom right
				((2 * ClusterZ + 1) * 2 * XDim) + (2 * ClusterX), // upper left
				((2 * ClusterZ + 1) * 2 * XDim) + (2 * ClusterX + 1), // upper right
			};

			for (int Quadrant = 0 ; Quadrant < 4 ; ++Quadrant)
			{
				uint32 CellID = CellIDs[Quadrant];

				// Query the cell's neighborhood to sum its alive neighbors
				uint8 AliveNeighbors = 0;
				uint32 NeighborhoodStart = 8 * CellID;
				for (uint8 NeighborID = 0; NeighborID < 8; ++NeighborID)
				{
					int32 CandidateCellID = Neighbors->operator[](NeighborhoodStart + NeighborID);
					AliveNeighbors += int(CurrentStates->operator[](CandidateCellID));
				}

				// Apply automata rules
				if ((*CurrentStates)[CellID])
				{ // Any live cell with appropriate amount of neighbors survives
					(*NextStates)[CellID] = SurviveRules->Contains(AliveNeighbors);
				}
				else
				{ // Any dead cell with appropriate amount of neighbors becomes alive
					(*NextStates)[CellID] = BirthRules->Contains(AliveNeighbors);
				}

				int32 NextDataID = NumCustomData * ProcessorCluster + 2*Quadrant + 1;
				int32 TimeIndex = NumCustomData * ProcessorCluster + 8;

				//shift material state in time, so "Next" becomes "Current"
				ClusterInstance->PerInstanceSMCustomData[NextDataID - 1] = ClusterInstance->PerInstanceSMCustomData[NextDataID];

				// register change based on state
				if ((*NextStates)[CellID])
				{  // switch-off time is in the future, i.e. cell is still on
					ClusterInstance->PerInstanceSMCustomData[NextDataID] = NextStepTime + 3 * StepPeriod;
				}
				else // is off at next time
				{
					if ((*CurrentStates)[CellID])  // was previously on
					{ // register switch-off time as being upcoming step
						ClusterInstance->PerInstanceSMCustomData[NextDataID] = NextStepTime;
					}
					else // preserve old switch-off time
					{
						ClusterInstance->PerInstanceSMCustomData[NextDataID] = ClusterInstance->PerInstanceSMCustomData[NextDataID - 1];
					}

				}

				// apply next time transition
				ClusterInstance->PerInstanceSMCustomData[TimeIndex] = NextStepTime;

			}

			
		});

		Driver->ProcessCompleted();
	}
