// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NBodySimModule.h"
#include "GameFramework/Actor.h"
#include "NBodySimTypesDefinitions.h"
#include "Config/SimulationConfig.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "SimulationEngine.generated.h"

UCLASS()
class NBODYSIMULATION_API ASimulationEngine : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASimulationEngine(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void InitBodies();

	// Update Bodies position on CPU.
	virtual void UpdateBodiesPosition(float DeltaTime);

	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Simulation")
	TObjectPtr<USimulationConfig> SimulationConfig;

 
	
private:
	/** Component to handle multiple instance of identical static mesh. */
	UPROPERTY(VisibleAnywhere, Instanced)
	TObjectPtr<UInstancedStaticMeshComponent> InstancedStaticMeshComponent;

	/** Store all the bodies data of the simulation. */
	FNBodySimParameters SimParameters;
	
	/** Store the transform of all body of the simulation. */
	UPROPERTY()
	TArray<FTransform> BodyTransforms;
};
