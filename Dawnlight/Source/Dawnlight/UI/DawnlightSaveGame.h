// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DawnlightUITypes.h"
#include "DawnlightSaveGame.generated.h"

/**
 * 設定データの保存用クラス
 *
 * ゲーム設定（オーディオ、グラフィック、操作など）を
 * ディスクに永続化するためのSaveGameクラス
 */
UCLASS()
class DAWNLIGHT_API UDawnlightSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UDawnlightSaveGame();

	/** 保存スロット名 */
	static const FString SaveSlotName;

	/** ユーザーインデックス */
	static const int32 UserIndex;

	// ========================================================================
	// 保存データ
	// ========================================================================

	/** 全設定データ */
	UPROPERTY(VisibleAnywhere, Category = "設定")
	FDawnlightAllSettings Settings;

	/** 設定バージョン（マイグレーション用） */
	UPROPERTY(VisibleAnywhere, Category = "メタデータ")
	int32 SettingsVersion;

	/** 最終保存日時 */
	UPROPERTY(VisibleAnywhere, Category = "メタデータ")
	FDateTime LastSaveTime;

	// ========================================================================
	// 静的ヘルパー関数
	// ========================================================================

	/** デフォルト設定を取得 */
	static FDawnlightAllSettings GetDefaultSettings();

	/** 現在のバージョン番号 */
	static int32 GetCurrentVersion() { return 1; }
};
