// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlitchEffectWidget.generated.h"

class UImage;
class UMaterialInstanceDynamic;
class USoundBase;

/**
 * グリッチエフェクトの種類
 */
UENUM(BlueprintType)
enum class EGlitchType : uint8
{
	None,			// なし
	Minor,			// 軽微（わずかなノイズ）
	Moderate,		// 中程度（断続的な乱れ）
	Severe,			// 重度（激しい歪み）
	SystemAwareness	// システム覚醒（覚醒する監視イベント用）
};

/**
 * グリッチエフェクトウィジェット
 *
 * 「覚醒する監視」イベントなどで使用するグリッチ/スタティック効果
 * - 画面の乱れ
 * - RGBずれ
 * - ノイズバー
 * - ブロックノイズ
 * - 水平/垂直シフト
 */
UCLASS()
class DAWNLIGHT_API UGlitchEffectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGlitchEffectWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// グリッチ制御
	// ========================================================================

	/** グリッチをトリガー */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void TriggerGlitch(EGlitchType Type, float Duration = 0.5f);

	/** システム覚醒グリッチ（特殊イベント用） */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void TriggerSystemAwareness(float Duration = 3.0f);

	/** 継続グリッチを開始 */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void StartContinuousGlitch(EGlitchType Type);

	/** 継続グリッチを停止 */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void StopContinuousGlitch();

	/** グリッチ強度を直接設定 (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void SetGlitchIntensity(float Intensity);

	/** 瞬間的なスタティックバースト */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void TriggerStaticBurst(float Duration = 0.1f);

	/** RGBシフトをトリガー */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void TriggerRGBShift(float Duration = 0.2f);

	/** 画面シフトをトリガー */
	UFUNCTION(BlueprintCallable, Category = "グリッチ")
	void TriggerScreenShift(float Duration = 0.15f);

	// ========================================================================
	// 取得
	// ========================================================================

	UFUNCTION(BlueprintPure, Category = "グリッチ")
	bool IsGlitching() const { return bIsGlitching || bIsContinuousGlitch; }

	UFUNCTION(BlueprintPure, Category = "グリッチ")
	EGlitchType GetCurrentGlitchType() const { return CurrentGlitchType; }

	UFUNCTION(BlueprintPure, Category = "グリッチ")
	float GetGlitchIntensity() const { return CurrentGlitchIntensity; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素
	// ========================================================================

	/** メイングリッチオーバーレイ */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> GlitchOverlay;

	/** ノイズオーバーレイ */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> NoiseOverlay;

	/** RGBシフトオーバーレイ */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> RGBShiftOverlay;

	/** ブロックノイズオーバーレイ */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> BlockNoiseOverlay;

	// ========================================================================
	// マテリアル
	// ========================================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|マテリアル")
	TObjectPtr<UMaterialInterface> GlitchMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|マテリアル")
	TObjectPtr<UMaterialInterface> NoiseMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|マテリアル")
	TObjectPtr<UMaterialInterface> RGBShiftMaterial;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 軽微グリッチの強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinorIntensity = 0.2f;

	/** 中程度グリッチの強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ModerateIntensity = 0.5f;

	/** 重度グリッチの強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SevereIntensity = 0.8f;

	/** システム覚醒の強度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SystemAwarenessIntensity = 1.0f;

	/** RGBシフトの最大オフセット */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定")
	float MaxRGBOffset = 10.0f;

	/** 画面シフトの最大オフセット */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定")
	float MaxScreenShift = 20.0f;

	/** ノイズバーの高さ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定")
	float NoiseBarHeight = 50.0f;

	/** ブロックノイズのサイズ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定")
	float BlockNoiseSize = 32.0f;

	/** グリッチ発生確率（継続モード時） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GlitchProbability = 0.3f;

	/** グリッチの最小間隔（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|設定", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float MinGlitchInterval = 0.05f;

	// ========================================================================
	// サウンド
	// ========================================================================

	/** グリッチ音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|サウンド")
	TObjectPtr<USoundBase> GlitchSound;

	/** スタティックノイズ音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|サウンド")
	TObjectPtr<USoundBase> StaticSound;

	/** システム覚醒音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "グリッチ|サウンド")
	TObjectPtr<USoundBase> SystemAwarenessSound;

private:
	/** グリッチ中フラグ */
	bool bIsGlitching;

	/** 継続グリッチ中フラグ */
	bool bIsContinuousGlitch;

	/** 現在のグリッチタイプ */
	EGlitchType CurrentGlitchType;

	/** 現在のグリッチ強度 */
	float CurrentGlitchIntensity;

	/** グリッチ残り時間 */
	float GlitchTimeRemaining;

	/** スタティックバースト残り時間 */
	float StaticBurstTimeRemaining;

	/** RGBシフト残り時間 */
	float RGBShiftTimeRemaining;

	/** 画面シフト残り時間 */
	float ScreenShiftTimeRemaining;

	/** 次のグリッチまでの時間 */
	float NextGlitchTimer;

	/** 現在のRGBオフセット */
	FVector2D CurrentRGBOffset;

	/** 現在の画面シフト */
	FVector2D CurrentScreenShift;

	/** ノイズシード */
	float NoiseSeed;

	/** 動的マテリアルインスタンス */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> GlitchMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> NoiseMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RGBShiftMID;

	/** マテリアルインスタンスを作成 */
	void CreateMaterialInstances();

	/** グリッチを更新 */
	void UpdateGlitch(float DeltaTime);

	/** 継続グリッチを更新 */
	void UpdateContinuousGlitch(float DeltaTime);

	/** エフェクトを更新 */
	void UpdateEffects(float DeltaTime);

	/** マテリアルパラメータを更新 */
	void UpdateMaterialParameters();

	/** グリッチタイプに基づいて強度を取得 */
	float GetIntensityForType(EGlitchType Type) const;

	/** ランダムなグリッチ効果を適用 */
	void ApplyRandomGlitchEffect();

	/** オーバーレイの可視性を更新 */
	void UpdateOverlayVisibility();

	/** サウンドを再生 */
	void PlayGlitchSound(EGlitchType Type);
};
