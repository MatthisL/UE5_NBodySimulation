#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/*
 * 
 */
class BASEPLUGINMODULE_API FBasePluginModule final : public IModuleInterface
{
public:
	static inline FBasePluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FBasePluginModule>("BasePluginModule");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("BasePluginModule");
	}
};
