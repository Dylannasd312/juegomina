using UnrealBuildTool;

public class Juego_mina : ModuleRules
{
	public Juego_mina(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayStateTree",
			"GameplayTags",
			"AIModule",
			"NavigationSystem",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate",
			"SlateCore",
			"AnimationCore",
			"PhysicsCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			ModuleDirectory + "/Public"
		});
	}
}