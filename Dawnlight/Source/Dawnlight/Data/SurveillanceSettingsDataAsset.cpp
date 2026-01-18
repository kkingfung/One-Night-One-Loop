// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurveillanceSettingsDataAsset.h"

USurveillanceSettingsDataAsset::USurveillanceSettingsDataAsset()
{
	// ========================================================================
	// 検知設定のデフォルト値
	// ========================================================================
	BaseDetectionRate = 0.3f;          // 約3.3秒で検知完了
	DetectionDecayRate = 0.15f;        // 約6.6秒で検知解除
	DetectionWarningThreshold = 0.5f;  // 50%で警告開始
	DetectionCompleteThreshold = 0.8f; // 80%で検知完了

	// ========================================================================
	// 光源設定のデフォルト値
	// ========================================================================
	MinLightIntensity = 0.1f;          // 10%以下の光は無視
	MaxLightDistance = 2000.0f;        // 最大検知距離
	SpotlightInnerConeMultiplier = 1.0f;
	SpotlightOuterConeMultiplier = 0.5f;

	// ========================================================================
	// 監視レベルLow設定
	// ========================================================================
	LevelLow.DetectionThreshold = 0.0f;
	LevelLow.LightRangeMultiplier = 1.0f;
	LevelLow.DetectionSpeedMultiplier = 1.0f;
	LevelLow.PatrolFrequencyMultiplier = 1.0f;
	LevelLow.MusicIntensity = 0.0f;
	LevelLow.WarningColor = FLinearColor(0.2f, 0.8f, 0.2f, 1.0f); // 緑

	// ========================================================================
	// 監視レベルMedium設定
	// ========================================================================
	LevelMedium.DetectionThreshold = 25.0f;
	LevelMedium.LightRangeMultiplier = 1.2f;
	LevelMedium.DetectionSpeedMultiplier = 1.3f;
	LevelMedium.PatrolFrequencyMultiplier = 1.2f;
	LevelMedium.MusicIntensity = 0.33f;
	LevelMedium.WarningColor = FLinearColor(0.8f, 0.8f, 0.2f, 1.0f); // 黄

	// ========================================================================
	// 監視レベルHigh設定
	// ========================================================================
	LevelHigh.DetectionThreshold = 50.0f;
	LevelHigh.LightRangeMultiplier = 1.5f;
	LevelHigh.DetectionSpeedMultiplier = 1.6f;
	LevelHigh.PatrolFrequencyMultiplier = 1.5f;
	LevelHigh.MusicIntensity = 0.66f;
	LevelHigh.WarningColor = FLinearColor(0.8f, 0.4f, 0.2f, 1.0f); // オレンジ

	// ========================================================================
	// 監視レベルCritical設定
	// ========================================================================
	LevelCritical.DetectionThreshold = 75.0f;
	LevelCritical.LightRangeMultiplier = 2.0f;
	LevelCritical.DetectionSpeedMultiplier = 2.0f;
	LevelCritical.PatrolFrequencyMultiplier = 2.0f;
	LevelCritical.MusicIntensity = 1.0f;
	LevelCritical.WarningColor = FLinearColor(0.9f, 0.1f, 0.1f, 1.0f); // 赤

	// ========================================================================
	// タイマー設定のデフォルト値
	// ========================================================================
	DetectionCooldownTime = 5.0f;
	AlertClearTime = 30.0f;
	SurveillanceLevelDecayInterval = 60.0f; // 1分ごとにレベル低下判定
}

const FSurveillanceLevelSettings& USurveillanceSettingsDataAsset::GetSettingsForDetectionLevel(float DetectionLevel) const
{
	// 検知レベルに基づいて適切な設定を返す
	if (DetectionLevel >= LevelCritical.DetectionThreshold)
	{
		return LevelCritical;
	}
	else if (DetectionLevel >= LevelHigh.DetectionThreshold)
	{
		return LevelHigh;
	}
	else if (DetectionLevel >= LevelMedium.DetectionThreshold)
	{
		return LevelMedium;
	}

	return LevelLow;
}
