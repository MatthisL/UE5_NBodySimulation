// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BodyData.h"
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

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitBodies();
	virtual void UpdateBodiesVelocity(float DeltaTime);
	virtual void UpdateBodiesPosition(float DeltaTime);
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Simulation")
	TObjectPtr<USimulationConfig> SimulationConfig;

 
	
private:
	/** Component to handle multiple instance of identical static mesh. */
	UPROPERTY(VisibleAnywhere, Instanced)
	TObjectPtr<UInstancedStaticMeshComponent> InstancedStaticMeshComponent;

	/** Store all the bodies data of the simulation. */
	UPROPERTY()
	TArray<FBodyData> Bodies;

	/** Store the transform of all body of the simulation. */
	UPROPERTY()
	TArray<FTransform> BodyTransforms;

	/** Stores the Width and Height half so we have the bound values of the screen. */
	FVector2D HalfScreen;
};
