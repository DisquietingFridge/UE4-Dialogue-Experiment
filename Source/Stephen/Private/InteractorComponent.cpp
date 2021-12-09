// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorComponent.h"

// Sets default values for this component's properties
UInteractorComponent::UInteractorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInteractorComponent::Actor_Requests_Consideration(const AActor* Candidate)
{
}

// Called when the game starts
void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



void UInteractorComponent::Interact_With_Actor()
{
}

// Called every frame
void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

