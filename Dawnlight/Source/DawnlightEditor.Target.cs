// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DawnlightEditorTarget : TargetRules
{
	public DawnlightEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("Dawnlight");

		// インストール済みエンジンでビルドするために必要
		bOverrideBuildEnvironment = true;
	}
}
