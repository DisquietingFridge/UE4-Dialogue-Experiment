// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorComponent.h"
#include "CoreMinimal.h"
#include "InteractionInterface.h"


// Sets default values for this component's properties
UInteractorComponent::UInteractorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	this->bWantsInitializeComponent = true;
	// ...
}

void UInteractorComponent::Actor_Requests_Consideration(const AActor* Candidate)
{
}


// Called when the game starts
void UInteractorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Volume = GetWorld()->SpawnActor<AActor>(VolumeClass);
	Volume->GetRootComponent()->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	Volume->OnActorBeginOverlap.AddDynamic(this, &UInteractorComponent::On_Overlap);
	Volume->OnActorEndOverlap.AddDynamic(this, &UInteractorComponent::On_End_Overlap);

	// ...
	
}

void UInteractorComponent::BeginPlay()
{
	if (GetOwner()->InputComponent == nullptr) {
		GetOwner()->InputComponent = NewObject<UInputComponent>(GetOwner()); // initialize input component
	}
	
	GetOwner()->InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &UInteractorComponent::Interact_With_Actor);

}



void UInteractorComponent::Interact_With_Actor()
{
	for (auto& entry : Actor_Candidates)
	{
		for (auto& comp : entry->GetComponentsByInterface(UInteractionInterface::StaticClass()))
		{
			Cast<IInteractionInterface>(comp)->Interact();
		}
		break;
	}
}

void UInteractorComponent::On_Overlap_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{

	Actor_Candidates.Add(OtherActor);
}

void UInteractorComponent::On_End_Overlap_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{
	Actor_Candidates.Remove(OtherActor);
}

// Called every frame
void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



