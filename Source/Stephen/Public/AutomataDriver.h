// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "AutomataDriver.generated.h"

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

	UPROPERTY()
		UInstancedStaticMeshComponent* Cell_Instance;

	UPROPERTY(Blueprintable, EditAnywhere)
		float P = 0.4; // Probability when initializing that a cell will start off alive.

	UPROPERTY(Blueprintable, EditAnywhere)
		TSet<uint8> Birth;

	UPROPERTY(Blueprintable, EditAnywhere)
		TSet<uint8> Survive;



	TArray<bool> Previous_States;
	TArray<bool> Next_States;


	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Xdim = 10;
	UPROPERTY(Blueprintable, EditAnywhere)
		int32 Zdim = 10;

	UPROPERTY(Blueprintable, EditAnywhere)
		int32 offset = 10;

	UPROPERTY(Blueprintable, EditAnywhere) // time in seconds per automata step
		float period = 1;

	float freq;

	float theta = 0;

	FTimerHandle AutomataTimer;

	UFUNCTION()
		void StepComplete();




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
