
#pragma once

// #if defined(__cplusplus)
// 	#define UINT_TYPE	unsigned int
// 	#define INT_TYPE	int
// 	#define FLOAT2_TYPE	FVector2f
// #else
// 	#define UINT_TYPE	uint
// 	#define INT_TYPE	int
// 	#define FLOAT2_TYPE	float2
// #endif



struct FBodyData
{
	float Mass;
	FVector2f Position;
	FVector2f Velocity;

	FBodyData()
		: Mass(0.0f)
		, Position(FVector2f::Zero())
		, Velocity(FVector2f::Zero())
	{
	}
	
	FBodyData(float InMass, FVector2f InPosition, FVector2f InVelocity)
		: Mass(InMass)
		, Position(InPosition)
		, Velocity(InVelocity)
	{
	}
};


// #undef UINT_TYPE
// #undef INT_TYPE