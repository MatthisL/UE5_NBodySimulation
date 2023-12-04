#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/*
 * 
 */
class BASEPLUGIN_API FBasePluginModule : public IModuleInterface
{
public:
	static inline FBasePluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FBasePluginModule>("BasePlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("BasePlugin");
	}
};
