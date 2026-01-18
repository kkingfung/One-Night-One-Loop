// Copyright Epic Games, Inc. All Rights Reserved.

#include "NightProgressSettingsDataAsset.h"

UNightProgressSettingsDataAsset::UNightProgressSettingsDataAsset()
{
	// ========================================================================
	// 時間設定のデフォルト値
	// ========================================================================
	TotalNightDuration = 900.0f;  // 15分
	TimeDisplayMultiplier = 1.0f;
	NightStartHour = 22.0f;       // 午後10時
	NightEndHour = 6.0f;          // 午前6時

	// ========================================================================
	// 導入フェーズ（0% - 33%）
	// ========================================================================
	IntroductionPhase.PhaseName = FText::FromString(TEXT("導入"));
	IntroductionPhase.StartProgress = 0.0f;
	IntroductionPhase.BaseTension = 20.0f;
	IntroductionPhase.MaxTension = 40.0f;
	IntroductionPhase.EventFrequencyMultiplier = 0.8f;
	IntroductionPhase.MusicParameter = 0.0f;
	IntroductionPhase.AmbientLightIntensity = 0.4f;

	// ========================================================================
	// 緩和フェーズ（33% - 66%）
	// ========================================================================
	RelaxationPhase.PhaseName = FText::FromString(TEXT("緩和"));
	RelaxationPhase.StartProgress = 0.33f;
	RelaxationPhase.BaseTension = 35.0f;
	RelaxationPhase.MaxTension = 60.0f;
	RelaxationPhase.EventFrequencyMultiplier = 1.0f;
	RelaxationPhase.MusicParameter = 0.33f;
	RelaxationPhase.AmbientLightIntensity = 0.3f;

	// ========================================================================
	// クライマックスフェーズ（66% - 100%）
	// ========================================================================
	ClimaxPhase.PhaseName = FText::FromString(TEXT("クライマックス"));
	ClimaxPhase.StartProgress = 0.66f;
	ClimaxPhase.BaseTension = 60.0f;
	ClimaxPhase.MaxTension = 100.0f;
	ClimaxPhase.EventFrequencyMultiplier = 1.5f;
	ClimaxPhase.MusicParameter = 0.66f;
	ClimaxPhase.AmbientLightIntensity = 0.2f;

	// ========================================================================
	// イベント設定のデフォルト値
	// ========================================================================
	FixedEventMinInterval = 120.0f;      // 2分
	RandomEventBaseInterval = 60.0f;     // 1分
	RandomEventIntervalVariance = 30.0f; // ±30秒

	// ========================================================================
	// 警告設定のデフォルト値
	// ========================================================================
	DawnWarningTime = 60.0f;             // 残り1分で警告
	PhaseTransitionWarningTime = 10.0f;  // フェーズ移行10秒前に警告
}

int32 UNightProgressSettingsDataAsset::GetPhaseFromProgress(float Progress) const
{
	// 進行度に基づいてフェーズを決定
	if (Progress >= ClimaxPhase.StartProgress)
	{
		return 2; // クライマックス
	}
	else if (Progress >= RelaxationPhase.StartProgress)
	{
		return 1; // 緩和
	}

	return 0; // 導入
}

const FNightPhaseSettings& UNightProgressSettingsDataAsset::GetPhaseSettings(int32 Phase) const
{
	switch (Phase)
	{
	case 1:
		return RelaxationPhase;
	case 2:
		return ClimaxPhase;
	default:
		return IntroductionPhase;
	}
}

FString UNightProgressSettingsDataAsset::GetDisplayTimeFromProgress(float Progress) const
{
	// 夜の開始時刻から終了時刻までを進行度で補間
	// 22時から6時の場合、8時間 = 480分

	float TotalHours;
	if (NightEndHour > NightStartHour)
	{
		// 同日内（例: 0時から6時）
		TotalHours = NightEndHour - NightStartHour;
	}
	else
	{
		// 日をまたぐ（例: 22時から6時）
		TotalHours = (24.0f - NightStartHour) + NightEndHour;
	}

	// 現在の時刻を計算
	const float CurrentHour = NightStartHour + (TotalHours * Progress);
	float NormalizedHour = FMath::Fmod(CurrentHour, 24.0f);
	if (NormalizedHour < 0.0f)
	{
		NormalizedHour += 24.0f;
	}

	// 時と分に分解
	const int32 Hours = FMath::FloorToInt(NormalizedHour);
	const int32 Minutes = FMath::FloorToInt(FMath::Fmod(NormalizedHour, 1.0f) * 60.0f);

	return FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
}
