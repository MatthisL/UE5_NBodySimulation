
using UnrealBuildTool;

public class BasePlugin: ModuleRules
{
	public BasePlugin(ReadOnlyTargetRules Target) 
		: base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine"
		});
	}
}
