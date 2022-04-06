// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionInterface.h"
#include "Interactable.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEPHEN_API UInteractable : public UActorComponent, public IInteractionInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UActorComponent> ActiveComponentClass;
	UPROPERTY(BlueprintReadWrite) 
		UActorComponent* ActiveComponent = nullptr;

	UFUNCTION()
		virtual void Interact() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Blueprintable)
		void StartInteraction();
	virtual void StartInteraction_Implementation();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
