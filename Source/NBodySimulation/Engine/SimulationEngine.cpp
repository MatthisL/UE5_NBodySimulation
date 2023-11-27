// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationEngine.h"

#include "SimulationLogChannels.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASimulationEngine::ASimulationEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	
	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
}

// Called every frame
void ASimulationEngine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateBodiesVelocity(DeltaTime);
	UpdateBodiesPosition(DeltaTime);
}

// Called when the game starts or when spawned
void ASimulationEngine::BeginPlay()
{
	Super::BeginPlay();

	if (!SimulationConfig)
	{
		UE_LOG(LogNBodySimulation, Error, TEXT("Failed to start simulation : SimulationConfig data asset has not been assigned in simulation engine."));

		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
		return;
	}

	// Compute static variables.
	float ScreenHeight = SimulationConfig->CameraOrthoWidth / SimulationConfig->CameraAspectRatio;
	HalfScreen = FVector2D(SimulationConfig->CameraOrthoWidth / 2.0f, ScreenHeight / 2.0f);
	
	InitBodies();
}

void ASimulationEngine::InitBodies()
{
	check(InstancedStaticMeshComponent);
	check(SimulationConfig);
	
	if (!SimulationConfig->BodyStaticMesh)
	{
		UE_LOG(LogNBodySimulation, Warning, TEXT("No reference set for rendering static mesh body in SimulationConfig asset file \"%s\"."), *SimulationConfig.GetName());
	}

	InstancedStaticMeshComponent->SetStaticMesh(SimulationConfig->BodyStaticMesh);


	
	Bodies.SetNumUninitialized(SimulationConfig->NumberOfBody + SimulationConfig->CustomBodies.Num());
	BodyTransforms.SetNumUninitialized(SimulationConfig->NumberOfBody + SimulationConfig->CustomBodies.Num());
	
	// Initialize the random Bodies with a default random position, velocity and mass depending on config.
	for (int32 Index = 0; Index < SimulationConfig->NumberOfBody; ++Index)
	{
		float RandomMass = FMath::FRandRange(SimulationConfig->InitialBodyMassRange.X, SimulationConfig->InitialBodyMassRange.Y);

		FVector2D RandomPosition(FMath::RandPointInCircle(SimulationConfig->BodySpawnCircleRadius));

		/**
		 *	For the velocity, we need to have a starting velocity for each body so that they are rotating into the circle clockwise.
		 *	This allow to have a nice starting movement.
		 *	With 0 starting velocity, we kind of have a Supernova.
		 */
		float RadialSpeedFactor = SimulationConfig->BodySpawnCircleRadius / RandomPosition.Size();
		FVector2D RandomVelocity
		{
			FMath::FRandRange(SimulationConfig->BodySpawnVelocityRange.X, SimulationConfig->BodySpawnVelocityRange.Y) / RadialSpeedFactor,
			0
		};
		RandomVelocity = RandomVelocity.GetRotated(90.0f + FMath::RadiansToDegrees(FMath::Atan2(RandomPosition.Y, RandomPosition.X)));
		
		float MeshScale = FMath::Sqrt(RandomMass) * SimulationConfig->MeshScaling;
		
		FTransform MeshTransform(
			FRotator(),
			FVector(RandomPosition, 0.0f),
			FVector(MeshScale, MeshScale, 1.0f)
		);
		
		BodyTransforms[Index] = MeshTransform;
		Bodies[Index] = FBodyData(RandomMass, RandomPosition, RandomVelocity, Index);
	}

	// Initialize the additional bodies set in the config file.
	for (int32 Index = 0; Index < SimulationConfig->CustomBodies.Num(); ++Index)
	{
		FBodyConfigEntry CustomBodyEntry = SimulationConfig->CustomBodies[Index];
		
		float MeshScale = FMath::Sqrt(CustomBodyEntry.Mass) * SimulationConfig->MeshScaling;
		
		FTransform MeshTransform (
		FRotator(),
		FVector(CustomBodyEntry.SpawnPosition, 0.0f),
		FVector(MeshScale, MeshScale, 1.0f)
		);

		int32 BodyIndex = SimulationConfig->NumberOfBody + Index;
		BodyTransforms[BodyIndex] = MeshTransform;
		Bodies[BodyIndex] = FBodyData(CustomBodyEntry.Mass, CustomBodyEntry.SpawnPosition, CustomBodyEntry.SpawnVelocity, BodyIndex);
	}

	/** Finally add instances to component to spawn them. */
	InstancedStaticMeshComponent->AddInstances(BodyTransforms, false);
}

void ASimulationEngine::UpdateBodiesVelocity(float DeltaTime)
{
	ParallelFor(Bodies.Num(), [&] (int32 Index)
	{
		FVector2D Acceleration(0.0f, 0.0f);

		for (const FBodyData& AffectingBody : Bodies)
		{
			/** Exclude itself. */
			if (AffectingBody.Index == Bodies[Index].Index)
				continue;

			FVector2D GravityForce = Bodies[Index].CalculateGravitationalForce(AffectingBody, SimulationConfig->GravitationalConstant);
			Acceleration += GravityForce / Bodies[Index].Mass;			
		}
		
		Bodies[Index].Velocity += Acceleration * DeltaTime;
	});
}

void ASimulationEngine::UpdateBodiesPosition(float DeltaTime)
{
	check(SimulationConfig);
	
	for (FBodyData& Body : Bodies)
	{
		Body.Position += Body.Velocity * DeltaTime;

		/** Pop body to the opposite of the screen if going beyond of the screen bounds. */
		Body.Position.X = FMath::Wrap(Body.Position.X, -HalfScreen.X, HalfScreen.X);
		Body.Position.Y = FMath::Wrap(Body.Position.Y, -HalfScreen.Y, HalfScreen.Y);
		
		BodyTransforms[Body.Index].SetTranslation(FVector(Body.Position, 0.0f));
	}
	
	InstancedStaticMeshComponent->BatchUpdateInstancesTransforms(0, BodyTransforms, false, true);
}



