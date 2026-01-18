// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightUITypes.generated.h"

/**
 * UIの画面タイプ
 */
UENUM(BlueprintType)
enum class EDawnlightScreenType : uint8
{
	None			UMETA(DisplayName = "なし"),
	MainMenu		UMETA(DisplayName = "メインメニュー"),
	Settings		UMETA(DisplayName = "設定"),
	PauseMenu		UMETA(DisplayName = "ポーズメニュー"),
	Loading			UMETA(DisplayName = "ローディング"),
	Confirmation	UMETA(DisplayName = "確認ダイアログ")
};

/**
 * 設定カテゴリ
 */
UENUM(BlueprintType)
enum class ESettingsCategory : uint8
{
	Audio		UMETA(DisplayName = "オーディオ"),
	Graphics	UMETA(DisplayName = "グラフィック"),
	Controls	UMETA(DisplayName = "操作"),
	Gameplay	UMETA(DisplayName = "ゲームプレイ")
};

/**
 * グラフィック品質レベル
 */
UENUM(BlueprintType)
enum class EGraphicsQuality : uint8
{
	Low			UMETA(DisplayName = "低"),
	Medium		UMETA(DisplayName = "中"),
	High		UMETA(DisplayName = "高"),
	Ultra		UMETA(DisplayName = "ウルトラ"),
	Custom		UMETA(DisplayName = "カスタム")
};

/**
 * ウィンドウモード
 */
UENUM(BlueprintType)
enum class EWindowModeType : uint8
{
	Fullscreen			UMETA(DisplayName = "フルスクリーン"),
	WindowedFullscreen	UMETA(DisplayName = "ボーダーレス"),
	Windowed			UMETA(DisplayName = "ウィンドウ")
};

/**
 * オーディオ設定データ
 */
USTRUCT(BlueprintType)
struct FDawnlightAudioSettings
{
	GENERATED_BODY()

	/** マスター音量 (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "オーディオ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MasterVolume = 1.0f;

	/** BGM音量 (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "オーディオ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MusicVolume = 0.8f;

	/** 効果音音量 (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "オーディオ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SFXVolume = 1.0f;

	/** 環境音音量 (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "オーディオ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AmbientVolume = 0.7f;

	/** UIサウンド音量 (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "オーディオ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float UIVolume = 0.8f;
};

/**
 * グラフィック設定データ
 */
USTRUCT(BlueprintType)
struct FDawnlightGraphicsSettings
{
	GENERATED_BODY()

	/** ウィンドウモード */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック")
	EWindowModeType WindowMode = EWindowModeType::Fullscreen;

	/** 解像度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック")
	FIntPoint Resolution = FIntPoint(1920, 1080);

	/** 垂直同期 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック")
	bool bVSync = true;

	/** フレームレート上限 (0 = 無制限) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "0", ClampMax = "240"))
	int32 FrameRateLimit = 60;

	/** 全体的な品質レベル */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック")
	EGraphicsQuality OverallQuality = EGraphicsQuality::High;

	/** 影品質 (0-4) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "0", ClampMax = "4"))
	int32 ShadowQuality = 3;

	/** ポストプロセス品質 (0-4) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "0", ClampMax = "4"))
	int32 PostProcessQuality = 3;

	/** テクスチャ品質 (0-4) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "0", ClampMax = "4"))
	int32 TextureQuality = 3;

	/** アンチエイリアス品質 (0-4) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "0", ClampMax = "4"))
	int32 AntiAliasingQuality = 3;

	/** 視覚効果品質 (0-4) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "0", ClampMax = "4"))
	int32 EffectsQuality = 3;

	/** 明るさ補正 (-1.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float Brightness = 0.0f;

	/** ガンマ補正 (1.0 - 3.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "グラフィック", meta = (ClampMin = "1.0", ClampMax = "3.0"))
	float Gamma = 2.2f;
};

/**
 * 操作設定データ
 */
USTRUCT(BlueprintType)
struct FDawnlightControlSettings
{
	GENERATED_BODY()

	/** マウス感度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "操作", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float MouseSensitivity = 1.0f;

	/** Y軸反転 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "操作")
	bool bInvertY = false;

	/** 振動の有効化 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "操作")
	bool bEnableVibration = true;
};

/**
 * ゲームプレイ設定データ
 */
USTRUCT(BlueprintType)
struct FDawnlightGameplaySettings
{
	GENERATED_BODY()

	/** 字幕の表示 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ゲームプレイ")
	bool bShowSubtitles = true;

	/** 字幕サイズ (0.5 - 2.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ゲームプレイ", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float SubtitleScale = 1.0f;

	/** カメラシェイクの強度 (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ゲームプレイ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CameraShakeIntensity = 1.0f;

	/** ヒント表示の有効化 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ゲームプレイ")
	bool bShowHints = true;
};

/**
 * 全設定データ
 */
USTRUCT(BlueprintType)
struct FDawnlightAllSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定")
	FDawnlightAudioSettings Audio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定")
	FDawnlightGraphicsSettings Graphics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定")
	FDawnlightControlSettings Controls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "設定")
	FDawnlightGameplaySettings Gameplay;
};
