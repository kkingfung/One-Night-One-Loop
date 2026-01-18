// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DangerVignetteWidget.generated.h"

class UImage;
class UMaterialInstanceDynamic;

/**
 * 危険ビネットウィジェット
 *
 * 危険度に応じて画面周辺を暗くする/赤くする
 * - 監視レベルに連動
 * - ダメージ時の赤フラッシュ
 * - 隠れている時の安全表示
 * - 呼吸のようなパルス
 */
UCLASS()
class DAWNLIGHT_API UDangerVignetteWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDangerVignetteWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// ビネット制御
	// ========================================================================

	/** 危険レベルを設定 (0.0 = 安全, 1.0 = 最大危険) */
	UFUNCTION(BlueprintCallable, Category = "ビネット")
	void SetDangerLevel(float Level);

	/** 監視レベルを設定（検知ゲージと連動） */
	UFUNCTION(BlueprintCallable, Category = "ビネット")
	void SetSurveillanceLevel(float Level);

	/** 隠れ状態を設定 */
	UFUNCTION(BlueprintCallable, Category = "ビネット")
	void SetHidingState(bool bIsHiding);

	/** ビネット強度を直接設定 */
	UFUNCTION(BlueprintCallable, Category = "ビネット")
	void SetVignetteIntensity(float Intensity);

	/** ビネット色を設定 */
	UFUNCTION(BlueprintCallable, Category = "ビネット")
	void SetVignetteColor(const FLinearColor& Color);

	// ========================================================================
	// エフェクトトリガー
	// ========================================================================

	/** ダメージフラッシュ（赤） */
	UFUNCTION(BlueprintCallable, Category = "ビネット|エフェクト")
	void TriggerDamageFlash(float Intensity = 1.0f);

	/** 検知フラッシュ（黄/オレンジ） */
	UFUNCTION(BlueprintCallable, Category = "ビネット|エフェクト")
	void TriggerDetectionFlash();

	/** 安全フラッシュ（緑） */
	UFUNCTION(BlueprintCallable, Category = "ビネット|エフェクト")
	void TriggerSafeFlash();

	/** 呼吸パルスを開始 */
	UFUNCTION(BlueprintCallable, Category = "ビネット|エフェクト")
	void StartBreathingPulse();

	/** 呼吸パルスを停止 */
	UFUNCTION(BlueprintCallable, Category = "ビネット|エフェクト")
	void StopBreathingPulse();

	/** 心拍パルスをトリガー */
	UFUNCTION(BlueprintCallable, Category = "ビネット|エフェクト")
	void TriggerHeartbeatPulse();

	// ========================================================================
	// 取得
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "ビネット")
	float GetCurrentIntensity() const { return CurrentVignetteIntensity; }

	UFUNCTION(BlueprintPure, Category = "ビネット")
	FLinearColor GetCurrentColor() const { return CurrentVignetteColor; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素
	// ========================================================================

	/** ビネットオーバーレイ画像 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> VignetteImage;

	/** 追加フラッシュ用画像 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> FlashImage;

	// ========================================================================
	// マテリアル
	// ========================================================================

	/** ビネットマテリアル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|マテリアル")
	TObjectPtr<UMaterialInterface> VignetteMaterial;

	// ========================================================================
	// 色設定
	// ========================================================================

	/** 通常時の色（黒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|色")
	FLinearColor NormalColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/** 危険時の色（赤みがかった黒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|色")
	FLinearColor DangerColor = FLinearColor(0.15f, 0.0f, 0.0f, 1.0f);

	/** ダメージ時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|色")
	FLinearColor DamageFlashColor = FLinearColor(0.8f, 0.0f, 0.0f, 0.8f);

	/** 検知時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|色")
	FLinearColor DetectionFlashColor = FLinearColor(0.9f, 0.6f, 0.1f, 0.6f);

	/** 安全時の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|色")
	FLinearColor SafeFlashColor = FLinearColor(0.1f, 0.5f, 0.3f, 0.4f);

	/** 隠れ状態の色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|色")
	FLinearColor HidingColor = FLinearColor(0.0f, 0.02f, 0.05f, 1.0f);

	// ========================================================================
	// 強度設定
	// ========================================================================

	/** 基本ビネット強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|強度", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseVignetteIntensity = 0.3f;

	/** 最大ビネット強度（危険時） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|強度", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxVignetteIntensity = 0.7f;

	/** 隠れ状態のビネット強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|強度", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HidingVignetteIntensity = 0.5f;

	/** ビネット半径（0 = 中心から、1 = 端から） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|強度", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VignetteRadius = 0.5f;

	/** ビネットのソフトネス */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|強度", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VignetteSoftness = 0.4f;

	// ========================================================================
	// アニメーション設定
	// ========================================================================

	/** スムージング速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|アニメーション", meta = (ClampMin = "1.0", ClampMax = "20.0"))
	float SmoothSpeed = 5.0f;

	/** フラッシュの持続時間 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|アニメーション", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float FlashDuration = 0.3f;

	/** 呼吸パルスの速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|アニメーション")
	float BreathingSpeed = 1.5f;

	/** 呼吸パルスの振幅 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|アニメーション", meta = (ClampMin = "0.0", ClampMax = "0.3"))
	float BreathingAmplitude = 0.1f;

	/** 心拍パルスの速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ビネット|アニメーション")
	float HeartbeatSpeed = 4.0f;

private:
	/** 目標ビネット強度 */
	float TargetVignetteIntensity;

	/** 現在のビネット強度 */
	float CurrentVignetteIntensity;

	/** 目標ビネット色 */
	FLinearColor TargetVignetteColor;

	/** 現在のビネット色 */
	FLinearColor CurrentVignetteColor;

	/** 危険レベル */
	float CurrentDangerLevel;

	/** 監視レベル */
	float CurrentSurveillanceLevel;

	/** 隠れ状態 */
	bool bIsHiding;

	/** フラッシュタイマー */
	float FlashTimer;

	/** フラッシュ色 */
	FLinearColor FlashColor;

	/** フラッシュ強度 */
	float FlashIntensity;

	/** 呼吸パルス中 */
	bool bIsBreathing;

	/** 呼吸タイマー */
	float BreathingTimer;

	/** 心拍タイマー */
	float HeartbeatTimer;

	/** 動的マテリアルインスタンス */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> VignetteMID;

	/** マテリアルインスタンスを作成 */
	void CreateMaterialInstance();

	/** 目標値を計算 */
	void CalculateTargetValues();

	/** ビネットを更新 */
	void UpdateVignette(float DeltaTime);

	/** フラッシュを更新 */
	void UpdateFlash(float DeltaTime);

	/** 呼吸パルスを更新 */
	void UpdateBreathing(float DeltaTime);

	/** マテリアルパラメータを更新 */
	void UpdateMaterialParameters();
};
