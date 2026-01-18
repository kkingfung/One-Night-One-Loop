// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurveillanceSettingsDataAsset.generated.h"

/**
 * 監視レベル設定構造体
 *
 * 各監視レベルごとの設定値
 */
USTRUCT(BlueprintType)
struct FSurveillanceLevelSettings
{
	GENERATED_BODY()

	/** 検知レベルの閾値（この値以上で次のレベルに移行） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視設定", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float DetectionThreshold = 0.0f;

	/** 光の範囲乗数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視設定", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float LightRangeMultiplier = 1.0f;

	/** 検知速度乗数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視設定", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float DetectionSpeedMultiplier = 1.0f;

	/** パトロール頻度乗数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視設定", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float PatrolFrequencyMultiplier = 1.0f;

	/** BGM変化用のパラメータ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MusicIntensity = 0.0f;

	/** UIの警告色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視設定")
	FLinearColor WarningColor = FLinearColor::White;
};

/**
 * 監視システム設定データアセット
 *
 * 監視システム全体の設定を定義
 * - 検知速度
 * - 光の設定
 * - 監視レベルごとの設定
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API USurveillanceSettingsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	USurveillanceSettingsDataAsset();

	// ========================================================================
	// 検知設定
	// ========================================================================

	/** 光の中にいる時の検知速度（/秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知")
	float BaseDetectionRate;

	/** 検知レベルの減衰速度（/秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知")
	float DetectionDecayRate;

	/** 検知警告の閾値（0-1） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DetectionWarningThreshold;

	/** 検知完了の閾値（0-1） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DetectionCompleteThreshold;

	// ========================================================================
	// 光源設定
	// ========================================================================

	/** 光による検知の最小強度（これ以下は無視） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "光源")
	float MinLightIntensity;

	/** 光による検知の最大距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "光源")
	float MaxLightDistance;

	/** スポットライトの内角乗数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "光源")
	float SpotlightInnerConeMultiplier;

	/** スポットライトの外角乗数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "光源")
	float SpotlightOuterConeMultiplier;

	// ========================================================================
	// 監視レベル設定
	// ========================================================================

	/** 監視レベル Low の設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視レベル")
	FSurveillanceLevelSettings LevelLow;

	/** 監視レベル Medium の設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視レベル")
	FSurveillanceLevelSettings LevelMedium;

	/** 監視レベル High の設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視レベル")
	FSurveillanceLevelSettings LevelHigh;

	/** 監視レベル Critical の設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "監視レベル")
	FSurveillanceLevelSettings LevelCritical;

	// ========================================================================
	// タイマー設定
	// ========================================================================

	/** 検知後のクールダウン時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイマー")
	float DetectionCooldownTime;

	/** 警報解除までの時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイマー")
	float AlertClearTime;

	/** 監視レベル低下のインターバル（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイマー")
	float SurveillanceLevelDecayInterval;

	// ========================================================================
	// ヘルパー関数
	// ========================================================================

	/** 指定した検知値に対応する監視レベル設定を取得 */
	UFUNCTION(BlueprintPure, Category = "監視設定")
	const FSurveillanceLevelSettings& GetSettingsForDetectionLevel(float DetectionLevel) const;
};
