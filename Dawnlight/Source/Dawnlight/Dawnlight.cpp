// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dawnlight.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogDawnlight);

void FDawnlightModule::StartupModule()
{
	UE_LOG(LogDawnlight, Log, TEXT("Dawnlight モジュールを開始しました"));
}

void FDawnlightModule::ShutdownModule()
{
	UE_LOG(LogDawnlight, Log, TEXT("Dawnlight モジュールを終了しました"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDawnlightModule, Dawnlight, "Dawnlight");
