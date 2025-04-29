// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OneTwoBreakFree : ModuleRules
{
	public OneTwoBreakFree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PCG", "GeometryCollectionEngine", "PhysicsCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
