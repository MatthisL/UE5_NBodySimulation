
using UnrealBuildTool;

public class BasePluginModule: ModuleRules
{
	public BasePluginModule(ReadOnlyTargetRules Target) 
		: base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"RHI",
            "Slate"
		});
	}
}
