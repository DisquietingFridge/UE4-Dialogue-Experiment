// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AAutomataDriver;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "AutomataDriver.generated.h"



class STEPHEN_API CellProcessor : public FNonAbandonableTask
{
	friend class FAsyncTask<CellProcessor>;

public:

	CellProcessor(AAutomataDriver* Driver, UInstancedStaticMeshComponent* CellInstance, uint32 StartingIndex);

protected:
	class AAutomataDriver* Driver = nullptr;


	float StepPeriod;

	// Cell ID that the first Instance element corresponds to
	uint32 StartingIndex;
	uint32 NumElements;

	uint32 XDim;


	TSharedPtr<TSet<int32>> BirthRules = nullptr;
	TSharedPtr<TSet<int32>> SurviveRules = nullptr;

	UPROPERTY()
		UInstancedStaticMeshComponent* ClusterInstance;

	TSharedPtr<TArray<bool>> CurrentStates = nullptr;
	TSharedPtr<TArray<bool>> NextStates = nullptr;

	TSharedPtr<TArray<uint32>> Neighbors = nullptr;

public:
	void Initialize(TSharedPtr<AAutomataDriver> driver);
	void DoWork();


	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

UCLASS()
class STEPHEN_API AAutomataDriver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutomataDriver();



protected:
	// Called when the game starts or when spawned
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(Blueprintable, EditAnywhere)
		UStaticMesh* Mesh;

	UPROPERTY(Blueprintable, EditAnywhere)
		UMaterialInterface* Mat;


	UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Divisions = 2;

	TArray<FAsyncTask<CellProcessor>*> Processors;



	UPROPERTY()
		TArray<UInstancedStaticMeshComponent*> ClusterInstances;

	UPROPERTY(Blueprintable, EditAnywhere)
		float Probability = 0.4; // Probability when initializing that a cell will start off alive.

	UPROPERTY(Blueprintable, EditAnywhere)
		FString BirthString = TEXT("3");

	UPROPERTY(Blueprintable, EditAnywhere)
		FString SurviveString = TEXT("23");

	TSet<int32> BirthRules;
	TSet<int32> SurviveRules;



	TSharedPtr<TArray<bool>> CurrentStates = nullptr;
	TSharedPtr<TArray<bool>> NextStates = nullptr;

	TSharedPtr<TArray<uint32>> Neighbors = nullptr;

	void InitNeighborIndices();


	UPROPERTY(Blueprintable, EditAnywhere)
		uint32 XDim = 300;
	UPROPERTY(Blueprintable, EditAnywhere)
		uint32 ZDim = 300;

	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Offset = 200;

	UPROPERTY(Blueprintable, EditAnywhere) // time per step in seconds
		float StepPeriod = 0.01;

	UPROPERTY(Blueprintable, EditAnywhere) // exponent for switching off
		float PhaseExponent = 1;

		float Time = 0;

	UPROPERTY(Blueprintable, EditAnywhere) 
		float EmissiveMultiplier = 20;

	UPROPERTY(Blueprintable, EditAnywhere) // how many steps a dying cell takes to fade out
		float StepsToFade = 1000;

	FTimerHandle StepTimer;
	FTimerHandle InstanceUpdateTimer;

	UFUNCTION()
		void StepComplete();

	UFUNCTION()
		void UpdateInstance(uint32 Index);

	UFUNCTION()
		void TimerFired();

	uint32 CurrentProcess = 0;
	uint32 MaterialToUpdate = 0;




public:	

	void ProcessCompleted();


		TSharedPtr<TSet<int32>> GetBirthRules()
	{
		return MakeShareable(&(this->BirthRules));
	}

		TSharedPtr<TSet<int32>> GetSurviveRules()
	{
		return MakeShareable(&(this->SurviveRules));
	}

		TSharedPtr<TArray<bool>> GetCurrentStates()
	{
		return this->CurrentStates;
	}

		TSharedPtr<TArray<bool>> GetNextStates()
	{
		return this->NextStates;
	}

		TSharedPtr<TArray<uint32>> GetNeighbors()
		{
			return this->Neighbors;
		}

		UInstancedStaticMeshComponent* GetCellInstance(int Index) 
		{
			return ClusterInstances[Index];
		}

		float GetStepPeriod()
		{
			return StepPeriod;
		}

		float GetTime() 
		{
			return Time;
		}

		uint32 GetXDim()
		{
			return XDim;
		}

		


};
