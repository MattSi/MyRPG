// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyRPGGameMode.h"
#include "MyRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyRPGGameMode::AMyRPGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
