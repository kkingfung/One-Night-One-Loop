// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionGaugeWidget.generated.h"

class UProgressBar;
class UImage;
class UTextBlock;
class USoundBase;

/**
 * 検知状態
 */
UENUM(BlueprintType)
enum class EDetectionState : uint8
{
	Safe,		// 安全（緑/非表示）
	Caution,	// 注意（黄）
	Warning,	// 警戒（オレンジ）
	Danger,		// 危険（赤）
	Critical	// 臨界（赤点滅）
};

// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetectionStateChanged, EDetectionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFullyDetected);

/**
 * 検知ゲージウィジェット
 *
 * プレイヤーが光に入った時の検知レベルを表示
 * - パルス/グロー効果
 * - 状態に応じた色変化
 * - アイコンアニメーション
 * - サウンドフィードバック
 */
UCLASS()
class DAWNLIGHT_API UDetectionGaugeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDetectionGaugeWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// ゲージ制御
	// ========================================================================

	/** 検知レベルを設定 (0.0 = 安全, 1.0 = 完全検知) */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ")
	void SetDetectionLevel(float Level);

	/** 検知レベルを取得 */
	UFUNCTION(BlueprintPure, Category = "検知ゲージ")
	float GetDetectionLevel() const { return TargetDetectionLevel; }

	/** 現在の表示レベルを取得（スムージング適用後） */
	UFUNCTION(BlueprintPure, Category = "検知ゲージ")
	float GetDisplayLevel() const { return CurrentDetectionLevel; }

	/** 検知状態を取得 */
	UFUNCTION(BlueprintPure, Category = "検知ゲージ")
	EDetectionState GetDetectionState() const { return CurrentState; }

	/** ゲージをリセット */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ")
	void ResetGauge();

	// ========================================================================
	// 表示制御
	// ========================================================================

	/** ゲージを表示（フェードイン） */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ")
	void ShowGauge();

	/** ゲージを非表示（フェードアウト） */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ")
	void HideGauge();

	/** 常に表示するかどうかを設定 */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ")
	void SetAlwaysVisible(bool bAlwaysVisible);

	// ========================================================================
	// エフェクト
	// ========================================================================

	/** 警告パルスをトリガー */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ|エフェクト")
	void TriggerWarningPulse();

	/** 検知完了フラッシュ */
	UFUNCTION(BlueprintCallable, Category = "検知ゲージ|エフェクト")
	void TriggerDetectedFlash();

	// ========================================================================
	// デリゲート
	// ========================================================================

	/** 検知状態が変化した */
	UPROPERTY(BlueprintAssignable, Category = "検知ゲージ|イベント")
	FOnDetectionStateChanged OnDetectionStateChanged;

	/** 完全に検知された */
	UPROPERTY(BlueprintAssignable, Category = "検知ゲージ|イベント")
	FOnFullyDetected OnFullyDetected;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素
	// ========================================================================

	/** メインプログレスバー */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> MainProgressBar;

	/** グロープログレスバー（背景の光る効果） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> GlowProgressBar;

	/** 目のアイコン */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> EyeIcon;

	/** 状態テキスト */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatusText;

	/** 背景パネル */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> BackgroundPanel;

	// ========================================================================
	// 色設定
	// ========================================================================

	/** 安全時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|色")
	FLinearColor SafeColor = FLinearColor(0.1f, 0.6f, 0.3f, 1.0f);

	/** 注意時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|色")
	FLinearColor CautionColor = FLinearColor(0.9f, 0.8f, 0.2f, 1.0f);

	/** 警戒時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|色")
	FLinearColor WarningColor = FLinearColor(0.95f, 0.5f, 0.1f, 1.0f);

	/** 危険時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|色")
	FLinearColor DangerColor = FLinearColor(0.9f, 0.15f, 0.1f, 1.0f);

	/** 臨界時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|色")
	FLinearColor CriticalColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

	/** グロー色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|色")
	FLinearColor GlowColor = FLinearColor(1.0f, 0.3f, 0.2f, 0.5f);

	// ========================================================================
	// 閾値設定
	// ========================================================================

	/** 注意状態の閾値 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|閾値", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CautionThreshold = 0.25f;

	/** 警戒状態の閾値 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|閾値", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WarningThreshold = 0.5f;

	/** 危険状態の閾値 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|閾値", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DangerThreshold = 0.75f;

	/** 臨界状態の閾値 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|閾値", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CriticalThreshold = 0.9f;

	// ========================================================================
	// アニメーション設定
	// ========================================================================

	/** ゲージのスムージング速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|アニメーション", meta = (ClampMin = "1.0", ClampMax = "20.0"))
	float GaugeSmoothSpeed = 8.0f;

	/** パルス速度（臨界時） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|アニメーション")
	float CriticalPulseSpeed = 8.0f;

	/** グロー振幅 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|アニメーション", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlowAmplitude = 0.3f;

	/** アイコン揺れ強度（危険時） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|アニメーション")
	float IconShakeIntensity = 3.0f;

	/** フェード時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|アニメーション", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float FadeDuration = 0.3f;

	/** 安全時の自動非表示遅延 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|アニメーション", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float SafeHideDelay = 2.0f;

	// ========================================================================
	// サウンド
	// ========================================================================

	/** 状態上昇音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|サウンド")
	TObjectPtr<USoundBase> StateIncreaseSound;

	/** 臨界ループ音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|サウンド")
	TObjectPtr<USoundBase> CriticalLoopSound;

	/** 完全検知音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "検知ゲージ|サウンド")
	TObjectPtr<USoundBase> FullyDetectedSound;

private:
	/** 目標検知レベル */
	float TargetDetectionLevel;

	/** 現在の表示レベル（スムージング） */
	float CurrentDetectionLevel;

	/** 現在の状態 */
	EDetectionState CurrentState;

	/** パルスタイマー */
	float PulseTimer;

	/** 自動非表示タイマー */
	float AutoHideTimer;

	/** 常に表示フラグ */
	bool bAlwaysVisible;

	/** 表示中フラグ */
	bool bIsVisible;

	/** フェード中フラグ */
	bool bIsFading;

	/** フェードイン中か */
	bool bIsFadingIn;

	/** フェード進捗 */
	float FadeProgress;

	/** 警告パルスタイマー */
	float WarningPulseTimer;

	/** 検知フラッシュタイマー */
	float DetectedFlashTimer;

	/** ゲージを更新 */
	void UpdateGauge(float DeltaTime);

	/** 状態を更新 */
	void UpdateState();

	/** 色を更新 */
	void UpdateColors();

	/** パルスエフェクトを更新 */
	void UpdatePulseEffects(float DeltaTime);

	/** アイコンを更新 */
	void UpdateIcon(float DeltaTime);

	/** フェードを更新 */
	void UpdateFade(float DeltaTime);

	/** 自動非表示を更新 */
	void UpdateAutoHide(float DeltaTime);

	/** 現在の状態に基づいて色を取得 */
	FLinearColor GetStateColor() const;

	/** 状態変更時のサウンド再生 */
	void PlayStateChangeSound(EDetectionState NewState, EDetectionState OldState);
};
