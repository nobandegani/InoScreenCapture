/* Copyright (c) 2021-2024 by Inoland */

using System.IO;
using UnrealBuildTool;

public class ScreenCaptureLite : ModuleRules
{
	public ScreenCaptureLite(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "include")
			}
		);
		
		PublicAdditionalLibraries.Add("dxgi.lib");
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "screen_capture_lite_static.lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{

		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{

		}
	}
}