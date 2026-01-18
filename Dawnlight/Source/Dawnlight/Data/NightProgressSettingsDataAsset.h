// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NightProgressSettingsDataAsset.generated.h"

/**
 * フェーズ設定構造体
 *
 * 各フェーズ（導入・緩和・クライマックス）の設定
 */
USTRUCT(BlueprintType)
struct FNightPhaseSettings
{
	GENERATED_BODY()

	/** フェーズ名（日本語） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ")
	FText PhaseName;

	/** フェーズ開始時の夜の進行度（0-1） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StartProgress = 0.0f;

	/** 基本緊張度（この値を中心に変動） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BaseTension = 0.0f;

	/** 最大緊張度（このフェーズでの上限） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float MaxTension = 100.0f;

	/** イベント発生頻度乗数 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float EventFrequencyMultiplier = 1.0f;

	/** BGM用のパラメータ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MusicParameter = 0.0f;

	/** 環境光の明るさ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AmbientLightIntensity = 0.3f;
};

/**
 * 夜の進行設定データアセット
 *
 * 夜全体の進行に関する設定を定義
 * - 総時間
 * - フェーズ設定
 * - イベント間隔
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UNightProgressSettingsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UNightProgressSettingsDataAsset();

	// ========================================================================
	// 時間設定
	// ========================================================================

	/** 夜の総時間（秒）*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "時間")
	float TotalNightDuration;

	/** 時間表示用の変換係数（ゲーム内1秒 = 現実X秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "時間")
	float TimeDisplayMultiplier;

	/** 夜の開始時刻（表示用、24時間形式） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "時間")
	float NightStartHour;

	/** 夜の終了時刻（表示用、24時間形式） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "時間")
	float NightEndHour;

	// ========================================================================
	// フェーズ設定
	// ========================================================================

	/** 導入フェーズの設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ")
	FNightPhaseSettings IntroductionPhase;

	/** 緩和フェーズの設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ")
	FNightPhaseSettings RelaxationPhase;

	/** クライマックスフェーズの設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フェーズ")
	FNightPhaseSettings ClimaxPhase;

	// ========================================================================
	// イベント設定
	// ========================================================================

	/** 固定イベントの最小間隔（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	float FixedEventMinInterval;

	/** ランダムイベントの基本間隔（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	float RandomEventBaseInterval;

	/** ランダムイベント間隔のランダム幅（±秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イベント")
	float RandomEventIntervalVariance;

	// ========================================================================
	// 警告設定
	// ========================================================================

	/** 夜明け警告を出す残り時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "警告")
	float DawnWarningTime;

	/** フェーズ移行前の警告を出す時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "警告")
	float PhaseTransitionWarningTime;

	// ========================================================================
	// ヘルパー関数
	// ========================================================================

	/** 現在の進行度からフェーズを取得（0=導入, 1=緩和, 2=クライマックス） */
	UFUNCTION(BlueprintPure, Category = "夜の進行")
	int32 GetPhaseFromProgress(float Progress) const;

	/** 指定フェーズの設定を取得 */
	UFUNCTION(BlueprintPure, Category = "夜の進行")
	const FNightPhaseSettings& GetPhaseSettings(int32 Phase) const;

	/** 進行度から表示用時刻を計算 */
	UFUNCTION(BlueprintPure, Category = "夜の進行")
	FString GetDisplayTimeFromProgress(float Progress) const;
};
