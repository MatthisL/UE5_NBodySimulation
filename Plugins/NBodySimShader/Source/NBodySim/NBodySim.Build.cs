

namespace UnrealBuildTool.Rules
{
	public class NBodySim : ModuleRules
	{
		public NBodySim(ReadOnlyTargetRules Target)
			: base(Target)
		{			
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
			
			PrivateIncludePaths.AddRange(new string[] 
			{
				"NBodySim/Private",
				"../Shaders/Shared",
				"../Shaders/Public"
			});

			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Renderer",
				"RenderCore",
				"RHI",
				"Projects"
			});
		}
	}
}