// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/BodyData.h"
#include "UObject/Object.h"
#include "SimulationConfig.generated.h"

USTRUCT(BlueprintType)
struct FBodyConfigEntry
{
	GENERATED_BODY()

	/** The mass of this body. */
	UPROPERTY(EditAnywhere)
	float Mass;

	/** The spawn 2D position of this body in the simulation's screen. */
	UPROPERTY(EditAnywhere)
	FVector2D SpawnPosition;

	/** The spawn 2D velocity of this body in the simulation's screen. */
	UPROPERTY(EditAnywhere)
	FVector2D SpawnVelocity;
};


/**
 *	Asset file containing the configuration data for the simulation.
 */
UCLASS(BlueprintType)
class NBODYSIMULATION_API USimulationConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The number of bodies to handle in the simulation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Body")
	int32 NumberOfBody = 100.0f;

	/** Bodies will be spawned with a random mass included within this range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Body")
	FVector2D InitialBodyMassRange = FVector2D(20.0f, 50.0f);
	
	/** Radius of the circle in which bodies will be spawn randomly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Body")
	float BodySpawnCircleRadius = 1000.0f;

	/** Bodies will be spawned with a random velocity included within this range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Body")
	FVector2D BodySpawnVelocityRange = FVector2D(400.0f, 600.0f);

	/** You can add body manually with custom setup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Body")
	TArray<FBodyConfigEntry> CustomBodies;


	
	/** The gravitational constant value. Cannot be less than 1.0 to avoid diving by zero. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WorldSettings", meta = (ClampMin = 1.0f))
	float GravitationalConstant = 1000.0f;

	
	
	/** The static mesh used for rendering the bodies in the simulation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	TObjectPtr<UStaticMesh> BodyStaticMesh;

	/** Scale rate of the rendered bodies' static mesh knowing that mesh already scale depending on the body's mass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	float MeshScaling = 0.2f;

	/** The orthogonal camera's width configuration to wrap bodies when going out of screen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	float CameraOrthoWidth = 8000.0f;

	/** The main camera's aspect ratio. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	float CameraAspectRatio = 1.777778f;
};
