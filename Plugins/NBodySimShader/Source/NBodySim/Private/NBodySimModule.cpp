

#include "NBodySimModule.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "RHI.h"
#include "GlobalShader.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "NBodySimCS.h"

IMPLEMENT_MODULE(FNBodySimModule, NBodySim)

// Declare some GPU stats so we can track them later
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Render, TEXT("ShaderPlugin: Root Render"));
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Compute, TEXT("ShaderPlugin: Render Compute Shader"));
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Pixel, TEXT("ShaderPlugin: Render Pixel Shader"));

void FNBodySimModule::StartupModule()
{
	OnPostResolvedSceneColorHandle.Reset();
	bCachedParametersValid = false;

	// Maps virtual shader source directory to the plugin's actual shaders directory.
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("NBodySimShader"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/NBodySimShaders"), PluginShaderDir);
}

void FNBodySimModule::ShutdownModule()
{
	EndRendering();
}

void FNBodySimModule::BeginRendering()
{
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	bCachedParametersValid = false;

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &FNBodySimModule::PostResolveSceneColor_RenderThread);
	}
}

void FNBodySimModule::EndRendering()
{
	if (!OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	OnPostResolvedSceneColorHandle.Reset();

	CSBuffers.Release();
}

void FNBodySimModule::InitWithParameters(FNBodySimParameters& SimParameters)
{
	CachedNBodySimParameters = SimParameters;
	bCachedParametersValid = true;
}

void FNBodySimModule::UpdateDeltaTime(float DeltaTime)
{
	RenderEveryFrameLock.Lock();
	CachedNBodySimParameters.DeltaTime = DeltaTime;
	RenderEveryFrameLock.Unlock();
}

void FNBodySimModule::PostResolveSceneColor_RenderThread(FRDGBuilder& Builder, const FSceneTextures& SceneTexture)
{
	if (!bCachedParametersValid)
	{
		return;
	}
	
	// Depending on your data, you might not have to lock here, just added this code to show how you can do it if you have to.
	RenderEveryFrameLock.Lock();
	FNBodySimParameters Copy = CachedNBodySimParameters;
	RenderEveryFrameLock.Unlock();

	ComputeSimulation_RenderThread(CachedNBodySimParameters);
}

void FNBodySimModule::ComputeSimulation_RenderThread(FNBodySimParameters& SimParameters)
{
	check(IsInRenderingThread());
	
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_ComputeSimulation); // Used to gather CPU profiling data for the UE session frontend
	SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_ComputeSimulation); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	CSBuffers.Initialize(SimParameters);
	
	FNBodySimCSInterface::RunComputeBodyPositions_RenderThread(RHICmdList, SimParameters, CSBuffers);

	RenderEveryFrameLock.Lock();
	{
		// Get readback data into CPU readable struct.
		void* RawBufferData = RHILockBuffer(CSBuffers.PositionsBuffer, 0, SimParameters.NumBodies * sizeof(FVector2f), RLM_ReadOnly);

		if (OutputPositions.Num() != SimParameters.NumBodies)
			OutputPositions.SetNumUninitialized(SimParameters.NumBodies);
		
		FMemory::Memcpy(OutputPositions.GetData(), RawBufferData, SimParameters.NumBodies * sizeof(FVector2f));
	
		RHIUnlockBuffer(CSBuffers.PositionsBuffer);
	}
	RenderEveryFrameLock.Unlock();
}
