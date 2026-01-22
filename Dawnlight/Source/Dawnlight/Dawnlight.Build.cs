// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Dawnlight : ModuleRules
{
	public Dawnlight(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// モジュールのルートディレクトリをインクルードパスに追加
		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory,  // ルートディレクトリ（Dawnlight.h用）
			Path.Combine(ModuleDirectory, "Abilities"),
			Path.Combine(ModuleDirectory, "Actors"),
			Path.Combine(ModuleDirectory, "AI"),
			Path.Combine(ModuleDirectory, "Characters"),
			Path.Combine(ModuleDirectory, "Combat"),
			Path.Combine(ModuleDirectory, "Components"),
			Path.Combine(ModuleDirectory, "Core"),
			Path.Combine(ModuleDirectory, "Data"),
			Path.Combine(ModuleDirectory, "Subsystems"),
			Path.Combine(ModuleDirectory, "UI"),
			Path.Combine(ModuleDirectory, "UI", "Widgets"),
			Path.Combine(ModuleDirectory, "Utilities")
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"NetCore",
			"Niagara",
			"AIModule",
			"NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UMG",
			"RHI"
		});
	}
}
