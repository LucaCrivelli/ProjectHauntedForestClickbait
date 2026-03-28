// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HauntedForest_pg : ModuleRules
{
	public HauntedForest_pg(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"HauntedForest_pg",
			"HauntedForest_pg/Variant_Horror",
			"HauntedForest_pg/Variant_Horror/UI",
			"HauntedForest_pg/Variant_Shooter",
			"HauntedForest_pg/Variant_Shooter/AI",
			"HauntedForest_pg/Variant_Shooter/UI",
			"HauntedForest_pg/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
