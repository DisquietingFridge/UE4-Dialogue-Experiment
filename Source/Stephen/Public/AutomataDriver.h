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

	CellProcessor(AAutomataDriver* driver);

protected:
	class AAutomataDriver* driver = nullptr;

	int Xdim;
	int Zdim;

	TSharedPtr<TSet<int32>> BirthRules = nullptr;
	TSharedPtr<TSet<int32>> SurviveRules = nullptr;

	TSharedPtr<TArray<bool>> Previous_States = nullptr;
	TSharedPtr<TArray<bool>> Next_States = nullptr;

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
		UInstancedStaticMeshComponent* Cell_Instance;

	UPROPERTY(Blueprintable, EditAnywhere)
		float P = 0.4; // Probability when initializing that a cell will start off alive.

	UPROPERTY(Blueprintable, EditAnywhere)
		FString Birth;

	UPROPERTY(Blueprintable, EditAnywhere)
		FString Survive;

	TSet<int32> BirthRules;
	TSet<int32> SurviveRules;



	TSharedPtr<TArray<bool>> Previous_States = nullptr;
	TSharedPtr<TArray<bool>> Next_States = nullptr;
	//TArray<bool> Previous_States;
	//TArray<bool> Next_States;


	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Xdim = 10;
	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Zdim = 10;

	UPROPERTY(Blueprintable, EditAnywhere)
		int32 offset = 10;

	UPROPERTY(Blueprintable, EditAnywhere) // time per step in seconds
		float period = 1;

	UPROPERTY(Blueprintable, EditAnywhere) // exponent for switching off
		float phaseExponent = 1;


	UPROPERTY(Blueprintable, EditAnywhere) 
		float emissiveMultiplier = 20;

	UPROPERTY(Blueprintable, EditAnywhere) // how many steps a dying cell takes to fade out
		float stepsToFade = 5;

	FTimerHandle AutomataTimer;

	UFUNCTION()
		void StepComplete();




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		int getXdim()
	{
		return this->Xdim;
	}

	UFUNCTION()
		int getZdim()
	{
		return this->Zdim;
	}


		TSharedPtr<TSet<int32>> getBirthRules()
	{
		return MakeShareable(&(this->BirthRules));
	}


		TSharedPtr<TSet<int32>> getSurviveRules()
	{
		return MakeShareable(&(this->SurviveRules));
	}


		TSharedPtr<TArray<bool>> getPreviousStates()
	{
		return this->Previous_States;
	}


		TSharedPtr<TArray<bool>> getNextStates()
	{
		return this->Next_States;
	}


};
