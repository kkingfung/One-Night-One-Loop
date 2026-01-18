// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DawnlightUITypes.h"
#include "UISubsystem.generated.h"

class UDawnlightSaveGame;
class UUserWidget;

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsChanged, const FDawnlightAllSettings&, NewSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenChanged, EDawnlightScreenType, ScreenType);

/**
 * UI管理サブシステム
 *
 * アウトゲームUIの管理と設定の永続化を担当
 * - 画面遷移管理
 * - 設定の読み込み/保存
 * - グラフィック設定の適用
 * - オーディオ設定の適用
 */
UCLASS()
class DAWNLIGHT_API UUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ========================================================================
	// サブシステムライフサイクル
	// ========================================================================

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========================================================================
	// 画面管理
	// ========================================================================

	/** 現在表示中の画面タイプを取得 */
	UFUNCTION(BlueprintPure, Category = "UI")
	EDawnlightScreenType GetCurrentScreenType() const { return CurrentScreenType; }

	/** 画面タイプを設定（通知のみ、実際の表示はUI側で行う） */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetCurrentScreenType(EDawnlightScreenType NewScreenType);

	// ========================================================================
	// 設定管理
	// ========================================================================

	/** 現在の全設定を取得 */
	UFUNCTION(BlueprintPure, Category = "設定")
	FDawnlightAllSettings GetCurrentSettings() const { return CurrentSettings; }

	/** オーディオ設定を取得 */
	UFUNCTION(BlueprintPure, Category = "設定|オーディオ")
	FDawnlightAudioSettings GetAudioSettings() const { return CurrentSettings.Audio; }

	/** グラフィック設定を取得 */
	UFUNCTION(BlueprintPure, Category = "設定|グラフィック")
	FDawnlightGraphicsSettings GetGraphicsSettings() const { return CurrentSettings.Graphics; }

	/** 操作設定を取得 */
	UFUNCTION(BlueprintPure, Category = "設定|操作")
	FDawnlightControlSettings GetControlSettings() const { return CurrentSettings.Controls; }

	/** ゲームプレイ設定を取得 */
	UFUNCTION(BlueprintPure, Category = "設定|ゲームプレイ")
	FDawnlightGameplaySettings GetGameplaySettings() const { return CurrentSettings.Gameplay; }

	/** 全設定を適用 */
	UFUNCTION(BlueprintCallable, Category = "設定")
	void ApplySettings(const FDawnlightAllSettings& NewSettings);

	/** オーディオ設定を適用 */
	UFUNCTION(BlueprintCallable, Category = "設定|オーディオ")
	void ApplyAudioSettings(const FDawnlightAudioSettings& AudioSettings);

	/** グラフィック設定を適用 */
	UFUNCTION(BlueprintCallable, Category = "設定|グラフィック")
	void ApplyGraphicsSettings(const FDawnlightGraphicsSettings& GraphicsSettings);

	/** 操作設定を適用 */
	UFUNCTION(BlueprintCallable, Category = "設定|操作")
	void ApplyControlSettings(const FDawnlightControlSettings& ControlSettings);

	/** ゲームプレイ設定を適用 */
	UFUNCTION(BlueprintCallable, Category = "設定|ゲームプレイ")
	void ApplyGameplaySettings(const FDawnlightGameplaySettings& GameplaySettings);

	/** 設定をデフォルトにリセット */
	UFUNCTION(BlueprintCallable, Category = "設定")
	void ResetToDefaultSettings();

	/** カテゴリ別に設定をリセット */
	UFUNCTION(BlueprintCallable, Category = "設定")
	void ResetCategoryToDefault(ESettingsCategory Category);

	// ========================================================================
	// 設定の永続化
	// ========================================================================

	/** 設定をファイルに保存 */
	UFUNCTION(BlueprintCallable, Category = "設定")
	bool SaveSettings();

	/** 設定をファイルから読み込み */
	UFUNCTION(BlueprintCallable, Category = "設定")
	bool LoadSettings();

	/** 保存されたデータが存在するか確認 */
	UFUNCTION(BlueprintPure, Category = "設定")
	bool DoesSaveExist() const;

	// ========================================================================
	// グラフィックユーティリティ
	// ========================================================================

	/** 利用可能な解像度一覧を取得 */
	UFUNCTION(BlueprintCallable, Category = "設定|グラフィック")
	TArray<FIntPoint> GetAvailableResolutions() const;

	/** 品質プリセットを適用 */
	UFUNCTION(BlueprintCallable, Category = "設定|グラフィック")
	void ApplyQualityPreset(EGraphicsQuality Quality);

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 設定が変更されたときに発火 */
	UPROPERTY(BlueprintAssignable, Category = "設定|イベント")
	FOnSettingsChanged OnSettingsChanged;

	/** 画面が切り替わったときに発火 */
	UPROPERTY(BlueprintAssignable, Category = "UI|イベント")
	FOnScreenChanged OnScreenChanged;

private:
	/** 現在の画面タイプ */
	EDawnlightScreenType CurrentScreenType;

	/** 現在の設定 */
	FDawnlightAllSettings CurrentSettings;

	/** グラフィック設定をエンジンに適用 */
	void ApplyGraphicsToEngine(const FDawnlightGraphicsSettings& Settings);

	/** オーディオ設定をエンジンに適用 */
	void ApplyAudioToEngine(const FDawnlightAudioSettings& Settings);

	/** ウィンドウモードを変換 */
	EWindowMode::Type ConvertWindowMode(EWindowModeType Mode) const;
};
