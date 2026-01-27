// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "SoulParticleWidget.generated.h"

/**
 * 魂パーティクルのデータ
 */
USTRUCT(BlueprintType)
struct FSoulParticle
{
	GENERATED_BODY()

	/** 現在の位置 */
	FVector2D Position;

	/** 移動速度 */
	FVector2D Velocity;

	/** サイズ */
	float Size;

	/** 色 */
	FLinearColor Color;

	/** 透明度 */
	float Alpha;

	/** 点滅タイマー */
	float PulseTimer;

	/** 点滅速度 */
	float PulseSpeed;

	/** ライフタイム */
	float Lifetime;

	/** 最大ライフタイム */
	float MaxLifetime;

	/** アクティブかどうか */
	bool bActive;
};

/**
 * 魂パーティクルウィジェット
 *
 * メニュー画面の背景に浮遊する魂のパーティクルを描画
 * Soul Reaperの雰囲気を演出する装飾的なウィジェット
 *
 * 特徴:
 * - 紫系のグラデーションパーティクル
 * - ゆっくりと上昇する動き
 * - フェードイン/アウトするライフサイクル
 * - 点滅（パルス）エフェクト
 */
UCLASS()
class DAWNLIGHT_API USoulParticleWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	USoulParticleWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// 制御
	// ========================================================================

	/** パーティクル生成を開始 */
	UFUNCTION(BlueprintCallable, Category = "パーティクル")
	void StartEmission();

	/** パーティクル生成を停止 */
	UFUNCTION(BlueprintCallable, Category = "パーティクル")
	void StopEmission();

	/** 全パーティクルをクリア */
	UFUNCTION(BlueprintCallable, Category = "パーティクル")
	void ClearParticles();

	/** バースト発生（一度に多くのパーティクルを生成） */
	UFUNCTION(BlueprintCallable, Category = "パーティクル")
	void EmitBurst(int32 Count);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	// ========================================================================
	// 設定
	// ========================================================================

	/** 最大パーティクル数 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	int32 MaxParticles;

	/** 1秒あたりの発生数 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float EmissionRate;

	/** パーティクルの最小サイズ */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MinParticleSize;

	/** パーティクルの最大サイズ */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MaxParticleSize;

	/** パーティクルの最小ライフタイム */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MinLifetime;

	/** パーティクルの最大ライフタイム */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MaxLifetime;

	/** 上昇速度の最小値 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MinRiseSpeed;

	/** 上昇速度の最大値 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MaxRiseSpeed;

	/** 横揺れの強さ */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float SwayStrength;

	/** 点滅速度の最小値 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MinPulseSpeed;

	/** 点滅速度の最大値 */
	UPROPERTY(EditDefaultsOnly, Category = "設定")
	float MaxPulseSpeed;

	// ========================================================================
	// カラー設定
	// ========================================================================

	/** メインカラー（紫系） */
	UPROPERTY(EditDefaultsOnly, Category = "カラー")
	FLinearColor PrimaryColor;

	/** セカンダリカラー（金系） */
	UPROPERTY(EditDefaultsOnly, Category = "カラー")
	FLinearColor SecondaryColor;

	/** 金色パーティクルの出現確率（0-1） */
	UPROPERTY(EditDefaultsOnly, Category = "カラー", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GoldenParticleChance;

private:
	/** パーティクルリスト */
	TArray<FSoulParticle> Particles;

	/** 発生タイマー */
	float EmissionTimer;

	/** 発生中かどうか */
	bool bEmitting;

	/** 経過時間（揺れの計算用） */
	float TotalTime;

	/** 新しいパーティクルを生成 */
	void SpawnParticle();

	/** パーティクルを更新 */
	void UpdateParticles(float DeltaTime);

	/** パーティクルを描画 */
	void DrawParticle(const FSoulParticle& Particle, const FGeometry& AllottedGeometry,
		FSlateWindowElementList& OutDrawElements, int32 LayerId) const;
};
