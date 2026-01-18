// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDawnlight, Log, All);

/**
 * Dawnlight ゲームモジュール
 *
 * "One Night, One Loop" のメインゲームモジュール
 * 監視ホラーサバイバルゲームのコアシステムを提供
 */
class FDawnlightModule : public IModuleInterface
{
public:
	/** IModuleInterface の実装 */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
