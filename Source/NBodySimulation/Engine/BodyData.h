// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BodyData.generated.h"

/**
 *	Contains the data of a single body in the N-Body Simulation.
 */
USTRUCT(BlueprintType)
struct FBodyData
{
	GENERATED_BODY()

	/** The mass of this body. */
	UPROPERTY(VisibleAnywhere)
	float Mass;

	/** The 2D position of this body in the simulation's screen. */
	UPROPERTY(VisibleAnywhere)
	FVector2D Position;

	/** The 2D velocity of this body in the simulation's screen. */
	UPROPERTY(VisibleAnywhere)
	FVector2D Velocity;

	FVector2D Acceleration = FVector2D::Zero();

	UPROPERTY(VisibleAnywhere)
	int32 Index;


	FBodyData()
	{
		Mass = 1.0f;
		Position = FVector2D(0.0f, 0.0f);
		Velocity = FVector2D(0.0f, 0.0f);
		Index = 0;
	}

	FBodyData(float InMass, FVector2D InPosition, FVector2D InVelocity, int32 InIndex)
	{
		Mass = InMass;
		Position = InPosition;
		Velocity = InVelocity;
		Index = InIndex;
	}

	FVector2D CalculateGravitationalForce(const FBodyData& Other, double GravitationalConstant) const
	{
		FVector2D Direction = Other.Position - Position;
		Direction.Normalize();
		
		float Distance = FVector2D::Distance(Position, Other.Position);

		/**
		 *	Since we do not handle collision, we cant really compute small distance force.
		 *	This would make some body to get super high acceleration.
		 *	We also avoid diving by zero.
		 */
		Distance = FMath::Max(Distance, 100.0f);
		
		double ForceMagnitude = (GravitationalConstant * Mass * Other.Mass) / (Distance * Distance);
		return Direction * ForceMagnitude;
	}
};
