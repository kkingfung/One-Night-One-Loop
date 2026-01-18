// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NightProgressWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;

/**
 * 夜のフェーズ
 */
UENUM(BlueprintType)
enum class ENightPhase : uint8
{
	Introduction,	// 導入（低緊張）
	Relaxation,		// 緩和（中緊張）
	Climax			// 締め（再緊張）
};

/**
 * 夜進行バーウィジェット
 *
 * 夜の進行状況を視覚的に表示
 * - 月の満ち欠けアイコン
 * - フェーズインジケーター
 * - 時刻表示
 * - 夜明けまでのカウントダウン
 */
UCLASS()
class DAWNLIGHT_API UNightProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UNightProgressWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// 進行制御
	// ========================================================================

	/** 夜の進行度を設定 (0.0 = 夜開始, 1.0 = 夜明け) */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void SetNightProgress(float Progress);

	/** 現在のフェーズを設定 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void SetCurrentPhase(ENightPhase Phase);

	/** 時刻テキストを設定 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void SetTimeText(const FText& Text);

	/** 緊急警告を表示（夜明け間近） */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void ShowDawnWarning();

	/** イベント発生を通知（バーにパルス） */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void TriggerEventPulse();

	/** フェーズ移行演出 */
	UFUNCTION(BlueprintCallable, Category = "夜進行")
	void TriggerPhaseTransition();

	// ========================================================================
	// 取得
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "夜進行")
	float GetNightProgress() const { return CurrentProgress; }

	UFUNCTION(BlueprintPure, Category = "夜進行")
	ENightPhase GetCurrentPhase() const { return CurrentPhase; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素
	// ========================================================================

	/** 進行バー背景 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ProgressBarBackground;

	/** 進行バー本体 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar;

	/** 進行バーオーバーレイ（グロー用） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ProgressBarGlow;

	/** 月アイコン */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> MoonIcon;

	/** 太陽アイコン（夜明け用） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> SunIcon;

	/** 時刻テキスト */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeText;

	/** フェーズテキスト */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PhaseText;

	/** フェーズマーカー1（導入→緩和） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> PhaseMarker1;

	/** フェーズマーカー2（緩和→締め） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> PhaseMarker2;

	/** 警告オーバーレイ */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> WarningOverlay;

	// ========================================================================
	// 色設定
	// ========================================================================

	/** 導入フェーズの色（暗い青） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor IntroductionColor = FLinearColor(0.1f, 0.15f, 0.3f, 1.0f);

	/** 緩和フェーズの色（紫がかった青） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor RelaxationColor = FLinearColor(0.2f, 0.1f, 0.35f, 1.0f);

	/** 締めフェーズの色（赤みがかった暗い色） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor ClimaxColor = FLinearColor(0.3f, 0.1f, 0.15f, 1.0f);

	/** 夜明け間近の色（オレンジ） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor DawnColor = FLinearColor(0.9f, 0.5f, 0.2f, 1.0f);

	/** グロー色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor GlowColor = FLinearColor(0.4f, 0.6f, 0.9f, 0.5f);

	/** 警告色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor WarningColor = FLinearColor(0.9f, 0.6f, 0.1f, 0.8f);

	/** イベントパルス色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|色")
	FLinearColor EventPulseColor = FLinearColor(0.8f, 0.2f, 0.3f, 0.6f);

	// ========================================================================
	// フェーズ境界設定
	// ========================================================================

	/** 導入→緩和の境界（進行度） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|フェーズ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Phase1Boundary = 0.33f;

	/** 緩和→締めの境界（進行度） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|フェーズ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Phase2Boundary = 0.66f;

	/** 夜明け警告開始（進行度） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|フェーズ", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DawnWarningThreshold = 0.9f;

	// ========================================================================
	// アニメーション設定
	// ========================================================================

	/** 進行バーのスムージング速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|アニメーション")
	float ProgressSmoothSpeed = 2.0f;

	/** グローパルス速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|アニメーション")
	float GlowPulseSpeed = 1.5f;

	/** 月移動範囲（ピクセル） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|アニメーション")
	float MoonTravelDistance = 200.0f;

	/** フェーズ移行演出時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|アニメーション")
	float PhaseTransitionDuration = 1.0f;

	/** イベントパルス持続時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|アニメーション")
	float EventPulseDuration = 0.5f;

	/** 警告パルス速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|アニメーション")
	float WarningPulseSpeed = 4.0f;

	// ========================================================================
	// フェーズ名
	// ========================================================================

	/** 導入フェーズ名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|テキスト")
	FText IntroductionPhaseName = FText::FromString(TEXT("INTRODUCTION"));

	/** 緩和フェーズ名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|テキスト")
	FText RelaxationPhaseName = FText::FromString(TEXT("RELAXATION"));

	/** 締めフェーズ名 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "夜進行|テキスト")
	FText ClimaxPhaseName = FText::FromString(TEXT("CLIMAX"));

private:
	/** 現在の進行度 */
	float CurrentProgress;

	/** 表示用進行度（スムージング後） */
	float DisplayProgress;

	/** 現在のフェーズ */
	ENightPhase CurrentPhase;

	/** 現在のバー色 */
	FLinearColor CurrentBarColor;

	/** 目標バー色 */
	FLinearColor TargetBarColor;

	/** グロータイマー */
	float GlowTimer;

	/** イベントパルスタイマー */
	float EventPulseTimer;

	/** フェーズ移行タイマー */
	float PhaseTransitionTimer;

	/** 警告表示中 */
	bool bShowingWarning;

	/** 警告タイマー */
	float WarningTimer;

	/** 進行バーを更新 */
	void UpdateProgressBar(float DeltaTime);

	/** 色を更新 */
	void UpdateColors(float DeltaTime);

	/** グローエフェクトを更新 */
	void UpdateGlow(float DeltaTime);

	/** 月アイコンを更新 */
	void UpdateMoonIcon();

	/** イベントパルスを更新 */
	void UpdateEventPulse(float DeltaTime);

	/** フェーズ移行を更新 */
	void UpdatePhaseTransition(float DeltaTime);

	/** 警告を更新 */
	void UpdateWarning(float DeltaTime);

	/** 進行度に基づいて色を計算 */
	FLinearColor CalculateColorForProgress(float Progress) const;

	/** フェーズ名を取得 */
	FText GetPhaseName(ENightPhase Phase) const;
};
