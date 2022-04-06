// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractorComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STEPHEN_API UInteractorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> VolumeClass;
	UPROPERTY(BlueprintReadWrite) AActor* Volume = nullptr;


	UFUNCTION(BlueprintCallable)
	void Actor_Requests_Consideration(const AActor* Candidate);

protected:
	// Called when the game starts
	//virtual void BeginPlay() override;

	virtual void InitializeComponent() override;
	//virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void On_Overlap(AActor* OverlappedActor, AActor* OtherActor);
	virtual void On_Overlap_Implementation(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void On_End_Overlap(AActor* OverlappedActor, AActor* OtherActor);
	virtual void On_End_Overlap_Implementation(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(BlueprintReadWrite)
	TSet<AActor*> Actor_Candidates; // set of objects user could potentially interact with

	UFUNCTION(BlueprintCallable)
	void Interact_With_Actor(); // interact with the appropriate object

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
