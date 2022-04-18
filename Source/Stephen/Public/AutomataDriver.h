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

	CellProcessor(AAutomataDriver* Driver);

protected:
	class AAutomataDriver* Driver = nullptr;

	int XDim;
	int ZDim;


	TSharedPtr<TSet<int32>> BirthRules = nullptr;
	TSharedPtr<TSet<int32>> SurviveRules = nullptr;

	UPROPERTY()
		UInstancedStaticMeshComponent* CellInstance;

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
	virtual void BeginPlay() override;

	UPROPERTY(Blueprintable, EditAnywhere)
		UStaticMesh* Mesh;

	UPROPERTY(Blueprintable, EditAnywhere)
		UMaterialInterface* Mat;


	UMaterialInstanceDynamic* DynMaterial;

	FAsyncTask<CellProcessor>* Processor;



	UPROPERTY()
		UInstancedStaticMeshComponent* CellInstance;

	UPROPERTY(Blueprintable, EditAnywhere)
		float Probability = 0.4; // Probability when initializing that a cell will start off alive.

	UPROPERTY(Blueprintable, EditAnywhere)
		FString BirthString;

	UPROPERTY(Blueprintable, EditAnywhere)
		FString SurviveString;

	TSet<int32> BirthRules;
	TSet<int32> SurviveRules;



	TSharedPtr<TArray<bool>> CurrentStates = nullptr;
	TSharedPtr<TArray<bool>> NextStates = nullptr;

	TSharedPtr<TArray<uint32>> Neighbors = nullptr;



	UPROPERTY(Blueprintable, EditAnywhere)
		int32 XDim = 10;
	UPROPERTY(Blueprintable, EditAnywhere)
		int32 ZDim = 10;

	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Offset = 10;

	UPROPERTY(Blueprintable, EditAnywhere) // time per step in seconds
		float StepPeriod = 1;

	UPROPERTY(Blueprintable, EditAnywhere) // exponent for switching off
		float PhaseExponent = 1;


	UPROPERTY(Blueprintable, EditAnywhere) 
		float EmissiveMultiplier = 20;

	UPROPERTY(Blueprintable, EditAnywhere) // how many steps a dying cell takes to fade out
		float StepsToFade = 5;

	FTimerHandle AutomataTimer;

	UFUNCTION()
		void StepComplete();




public:	

	UFUNCTION()
		int GetXDim()
	{
		return this->XDim;
	}

	UFUNCTION()
		int GetZDim()
	{
		return this->ZDim;
	}


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

		UInstancedStaticMeshComponent* GetCellInstance() {
			return this->CellInstance;
		}

		void InitNeighborIndices();


};
