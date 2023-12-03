
#pragma once

#include "NBodySimCS.h"

#include "CanvasTypes.h"
#include "GlobalShader.h"
#include "PixelShaderUtils.h"
#include "RenderGraphResources.h"
#include "RHIGPUReadback.h"
#include "ShaderParameterStruct.h"
#include "StaticMeshResources.h"
#include "UnifiedBuffer.h"
#include "NBodySimModule.h"

DECLARE_STATS_GROUP(TEXT("NBodySimCS"), STATGROUP_NBodySimCS, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("NBodySimCS Execute"), STAT_NBodySimCS_Execute, STATGROUP_NBodySimCS);

/**********************************************************************************************/
/* This class carries our parameter declarations and acts as the bridge between cpp and HLSL. */
/**********************************************************************************************/
class FNBodySimCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FNBodySimCS);
	SHADER_USE_PARAMETER_STRUCT(FNBodySimCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_SRV(StructuredBuffer<float>, Masses)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FVector2f>, Positions)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FVector2f>, Velocities)
		SHADER_PARAMETER(uint32, NumBodies)
		SHADER_PARAMETER(float, GravityConstant)
		SHADER_PARAMETER(float, CameraAspectRatio)
		SHADER_PARAMETER(float, ViewportWidth)
		SHADER_PARAMETER(float, DeltaTime)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		// OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), 256);
		// OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
		// OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}
};

// This will tell the engine to create the shader and where the shader entry point is.
//                      ShaderType              ShaderPath                     Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FNBodySimCS, "/NBodySimShaders/Private/NBodySim.usf", "CalculateVelocitiesCS", SF_Compute);



void FNBodySimCSBuffers::Initialize(const FNBodySimParameters& SimParameters)
{
	if (!MassesBuffer || !MassesBufferSRV)
	{
		TResourceArray<float> ResourceArray;
		ResourceArray.Init(0.0f, SimParameters.Bodies.Num());

		for (int i = 0; i < SimParameters.Bodies.Num(); i++)
		{
			ResourceArray[i] = SimParameters.Bodies[i].Mass;
		}

		FRHIResourceCreateInfo CreateInfo(TEXT("RHICreateInfo_MassesBuffer"));
		CreateInfo.ResourceArray = &ResourceArray;

		MassesBuffer = RHICreateStructuredBuffer(sizeof(float), SimParameters.Bodies.Num() * sizeof(float), BUF_ShaderResource, CreateInfo);
		MassesBufferSRV = RHICreateShaderResourceView(MassesBuffer);
	}

	if (!PositionsBuffer || !PositionsBufferUAV)
	{
		TResourceArray<FVector2f> ResourceArray;
		ResourceArray.Init(FVector2f::ZeroVector, SimParameters.Bodies.Num());

		for (int i = 0; i < SimParameters.Bodies.Num(); i++)
		{
			ResourceArray[i] = SimParameters.Bodies[i].Position;
		}

		FRHIResourceCreateInfo CreateInfo(TEXT("RHICreateInfo_PositionsBuffer"));
		CreateInfo.ResourceArray = &ResourceArray;

		PositionsBuffer = RHICreateStructuredBuffer(sizeof(FVector2f), SimParameters.Bodies.Num() * sizeof(FVector2f), BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
		PositionsBufferUAV = RHICreateUnorderedAccessView(PositionsBuffer, false, true);
	}

	if (!VelocitiesBuffer || !VelocitiesBufferUAV)
	{
		TResourceArray<FVector2f> ResourceArray;
		ResourceArray.Init(FVector2f::ZeroVector, SimParameters.Bodies.Num());

		for (int i = 0; i < SimParameters.Bodies.Num(); i++)
		{
			ResourceArray[i] = SimParameters.Bodies[i].Velocity;
		}

		FRHIResourceCreateInfo CreateInfo(TEXT("RHICreateInfo_VelocitiesBuffer"));
		CreateInfo.ResourceArray = &ResourceArray;

		VelocitiesBuffer = RHICreateStructuredBuffer(sizeof(FVector2f), SimParameters.Bodies.Num() * sizeof(FVector2f), BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
		VelocitiesBufferUAV = RHICreateUnorderedAccessView(VelocitiesBuffer, false, true);
	}
}

void FNBodySimCSBuffers::Release()
{
	if (MassesBuffer)			MassesBuffer.SafeRelease();
	if (MassesBufferSRV)		MassesBufferSRV.SafeRelease();

	if (PositionsBuffer)		PositionsBuffer.SafeRelease();
	if (PositionsBufferUAV)	PositionsBufferUAV.SafeRelease();

	if (VelocitiesBuffer)		VelocitiesBuffer.SafeRelease();
	if (VelocitiesBufferUAV)	VelocitiesBufferUAV.SafeRelease();
}

void FNBodySimCSInterface::RunComputeBodyPositions_RenderThread(FRHICommandListImmediate& RHICmdList, FNBodySimParameters& SimParameters, FNBodySimCSBuffers Buffers)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_ComputeBodyPositions); // Used to gather CPU profiling data for the UE4 session frontend
	SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_ComputeBodyPositions); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	// Transition setup
	ERHIAccess BarrierAccess = ERHIAccess::CopySrc | ERHIAccess::CopyDest | ERHIAccess::SRVCompute | ERHIAccess::SRVGraphics | ERHIAccess::UAVCompute | ERHIAccess::UAVGraphics;
	
	FRHITransitionInfo TransitionInfoPositions(Buffers.PositionsBuffer, BarrierAccess);
	RHICmdList.Transition(TransitionInfoPositions);
		
	FRHITransitionInfo TransitionInfoVelocities(Buffers.VelocitiesBuffer, BarrierAccess);
	RHICmdList.Transition(TransitionInfoVelocities);

	// Shader Parameters setup.
	FNBodySimCS::FParameters PassParameters;
	PassParameters.Masses = Buffers.MassesBufferSRV;
	PassParameters.Positions = Buffers.PositionsBufferUAV;
	PassParameters.Velocities = Buffers.VelocitiesBufferUAV;

	PassParameters.NumBodies = SimParameters.NumBodies;
	PassParameters.GravityConstant = SimParameters.GravityConstant;
	PassParameters.CameraAspectRatio = SimParameters.CameraAspectRatio;
	PassParameters.ViewportWidth = SimParameters.ViewportWidth;
	PassParameters.DeltaTime = SimParameters.DeltaTime;


	// Dispatch.
	TShaderMapRef<FNBodySimCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	// FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(ComputeGroupSize(SimParameters.NumBodies), FComputeShaderUtils::kGolden2DGroupSize);

	FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, PassParameters, ComputeGroupSize(SimParameters.NumBodies));
}

FIntVector FNBodySimCSInterface::ComputeGroupSize(uint32 NumBodies)
{
	const int ThreadCount = 256;

	int FinalCount = ((NumBodies - 1) / ThreadCount) + 1;

	return FIntVector(FinalCount, 1, 1);
}
