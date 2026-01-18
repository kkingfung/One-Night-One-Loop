// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScanlineEffectWidget.generated.h"

class UImage;
class UMaterialInstanceDynamic;

/**
 * スキャンライン/CRTエフェクトウィジェット
 *
 * 監視カメラの雰囲気を演出するオーバーレイエフェクト
 * - 水平スキャンライン
 * - CRTカーブ歪み（オプション）
 * - ノイズ/スタティック
 * - 色収差
 * - フリッカー効果
 */
UCLASS()
class DAWNLIGHT_API UScanlineEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UScanlineEffectWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// エフェクト制御
	// ========================================================================

	/** エフェクトの強度を設定 (0.0 = オフ, 1.0 = 最大) */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetEffectIntensity(float Intensity);

	/** スキャンラインの強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetScanlineIntensity(float Intensity);

	/** ノイズの強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetNoiseIntensity(float Intensity);

	/** 色収差の強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetChromaticAberration(float Intensity);

	/** フリッカーの強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetFlickerIntensity(float Intensity);

	/** CRTカーブの強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetCRTCurve(float Intensity);

	/** ビネット（周辺減光）の強度を設定 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン")
	void SetVignetteIntensity(float Intensity);

	// ========================================================================
	// 状態連動
	// ========================================================================

	/** 監視レベルに応じてエフェクトを調整 (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン|連動")
	void SetSurveillanceLevel(float Level);

	/** 危険状態エフェクトを有効化 */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン|連動")
	void TriggerDangerPulse();

	/** グリッチエフェクトを一時的にトリガー */
	UFUNCTION(BlueprintCallable, Category = "スキャンライン|連動")
	void TriggerGlitch(float Duration = 0.3f);

	// ========================================================================
	// 取得
	// ========================================================================

	/** 現在のエフェクト強度を取得 */
	UFUNCTION(BlueprintPure, Category = "スキャンライン")
	float GetEffectIntensity() const { return CurrentEffectIntensity; }

	/** エフェクトが有効かどうか */
	UFUNCTION(BlueprintPure, Category = "スキャンライン")
	bool IsEffectEnabled() const { return CurrentEffectIntensity > 0.01f; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素
	// ========================================================================

	/** スキャンラインオーバーレイ画像 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ScanlineOverlay;

	/** ノイズオーバーレイ画像 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> NoiseOverlay;

	/** ビネットオーバーレイ画像 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> VignetteOverlay;

	// ========================================================================
	// マテリアル設定
	// ========================================================================

	/** スキャンラインマテリアル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|マテリアル")
	TObjectPtr<UMaterialInterface> ScanlineMaterial;

	/** ノイズマテリアル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|マテリアル")
	TObjectPtr<UMaterialInterface> NoiseMaterial;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 基本スキャンライン強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseScanlineIntensity = 0.15f;

	/** 基本ノイズ強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseNoiseIntensity = 0.05f;

	/** 基本色収差強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定", meta = (ClampMin = "0.0", ClampMax = "0.1"))
	float BaseChromaticAberration = 0.002f;

	/** 基本フリッカー強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseFlickerIntensity = 0.02f;

	/** スキャンラインの移動速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定")
	float ScanlineScrollSpeed = 50.0f;

	/** ノイズのアニメーション速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定")
	float NoiseAnimationSpeed = 30.0f;

	/** 監視レベルでのエフェクト増加率 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float SurveillanceEffectMultiplier = 2.5f;

	/** グリッチ時のノイズ強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スキャンライン|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlitchNoiseIntensity = 0.8f;

private:
	/** 現在のエフェクト強度 */
	float CurrentEffectIntensity;

	/** 現在のスキャンライン強度 */
	float CurrentScanlineIntensity;

	/** 現在のノイズ強度 */
	float CurrentNoiseIntensity;

	/** 現在の色収差強度 */
	float CurrentChromaticAberration;

	/** 現在のフリッカー強度 */
	float CurrentFlickerIntensity;

	/** 現在のビネット強度 */
	float CurrentVignetteIntensity;

	/** 現在のCRTカーブ強度 */
	float CurrentCRTCurve;

	/** スキャンラインのスクロールオフセット */
	float ScanlineOffset;

	/** ノイズシード（ランダム性のため） */
	float NoiseSeed;

	/** グリッチ残り時間 */
	float GlitchTimeRemaining;

	/** 危険パルスタイマー */
	float DangerPulseTimer;

	/** 危険パルス中かどうか */
	bool bIsDangerPulsing;

	/** 動的マテリアルインスタンス */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ScanlineMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> NoiseMID;

	/** マテリアルインスタンスを作成 */
	void CreateMaterialInstances();

	/** マテリアルパラメータを更新 */
	void UpdateMaterialParameters();

	/** フリッカー値を計算 */
	float CalculateFlicker() const;

	/** グリッチを更新 */
	void UpdateGlitch(float DeltaTime);

	/** 危険パルスを更新 */
	void UpdateDangerPulse(float DeltaTime);
};
