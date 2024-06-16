// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyRPG : ModuleRules
{
	public MyRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", 
				"EnhancedInput", "Niagara", "UMG", "AIModule", "NavigationSystem", "CableComponent" , "GameplayTasks"});

		PrivateDependencyModuleNames.AddRange(new string[] { "AnimGraphRuntime", "AnimationLocomotionLibraryRuntime" });

		PublicIncludePaths.AddRange(new string[] { "MyRPG" });
	}
}
