// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIAnimationLibrary.generated.h"

class UWidget;
class UWidgetAnimation;
class UUserWidget;

/**
 * アニメーションイージングタイプ
 */
UENUM(BlueprintType)
enum class EUIEaseType : uint8
{
	Linear        UMETA(DisplayName = "Linear"),
	EaseIn        UMETA(DisplayName = "Ease In"),
	EaseOut       UMETA(DisplayName = "Ease Out"),
	EaseInOut     UMETA(DisplayName = "Ease In/Out"),
	Bounce        UMETA(DisplayName = "Bounce"),
	Elastic       UMETA(DisplayName = "Elastic"),
	Back          UMETA(DisplayName = "Back (Overshoot)")
};

/**
 * UIアニメーションライブラリ
 *
 * Soul Reaperのメニュー画面でジューシーなアニメーションを
 * 簡単に適用するためのBlueprintライブラリ
 *
 * 機能:
 * - ボタンホバー/プレスエフェクト
 * - スケールアニメーション
 * - フェードアニメーション
 * - シェイク/バウンス効果
 * - カラーパルス効果
 */
UCLASS()
class DAWNLIGHT_API UUIAnimationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ========================================================================
	// イージング関数
	// ========================================================================

	/** イージング値を計算 */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Easing")
	static float CalculateEase(float Alpha, EUIEaseType EaseType);

	/** バウンスイージング */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Easing")
	static float EaseBounce(float Alpha);

	/** エラスティックイージング */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Easing")
	static float EaseElastic(float Alpha);

	/** バックイージング（オーバーシュート） */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Easing")
	static float EaseBack(float Alpha);

	// ========================================================================
	// スケールアニメーション
	// ========================================================================

	/** ウィジェットをスケールアニメーション（即時設定） */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Scale")
	static void SetWidgetScale(UWidget* Widget, FVector2D Scale);

	/** ポップイン効果（0→1に弾むようにスケール） */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Scale")
	static void AnimatePopIn(UWidget* Widget, float Duration = 0.3f);

	/** ポップアウト効果（1→0に縮小） */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Scale")
	static void AnimatePopOut(UWidget* Widget, float Duration = 0.2f);

	/** ホバー拡大効果 */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Scale")
	static void AnimateHoverScale(UWidget* Widget, bool bIsHovered, float Scale = 1.1f, float Duration = 0.15f);

	/** プレス縮小効果 */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Scale")
	static void AnimatePressScale(UWidget* Widget, bool bIsPressed, float Scale = 0.95f, float Duration = 0.08f);

	// ========================================================================
	// シェイク/バウンス効果
	// ========================================================================

	/** ウィジェットをシェイク */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Shake")
	static void AnimateShake(UWidget* Widget, float Intensity = 5.0f, float Duration = 0.3f);

	/** バウンス効果（軽く弾む） */
	UFUNCTION(BlueprintCallable, Category = "UI|Animation|Shake")
	static void AnimateBounce(UWidget* Widget, float Intensity = 10.0f, float Duration = 0.4f);

	// ========================================================================
	// カラー効果
	// ========================================================================

	/** Soul Reaperテーマの紫色を取得 */
	UFUNCTION(BlueprintPure, Category = "UI|Colors|SoulReaper")
	static FLinearColor GetSoulPurple();

	/** Soul Reaperテーマの金色を取得 */
	UFUNCTION(BlueprintPure, Category = "UI|Colors|SoulReaper")
	static FLinearColor GetSoulGold();

	/** Soul Reaperテーマのダーク色を取得 */
	UFUNCTION(BlueprintPure, Category = "UI|Colors|SoulReaper")
	static FLinearColor GetSoulDark();

	/** レアリティに基づく色を取得 */
	UFUNCTION(BlueprintPure, Category = "UI|Colors|SoulReaper")
	static FLinearColor GetRarityColor(int32 RarityLevel);

	/** 色をパルス（点滅）させる */
	UFUNCTION(BlueprintPure, Category = "UI|Colors")
	static FLinearColor PulseColor(FLinearColor BaseColor, FLinearColor TargetColor, float Time, float Speed = 2.0f);

	/** グラデーション補間 */
	UFUNCTION(BlueprintPure, Category = "UI|Colors")
	static FLinearColor LerpGradient(const TArray<FLinearColor>& Colors, float Alpha);

	// ========================================================================
	// ユーティリティ
	// ========================================================================

	/** ランダムな角度（揺れ効果用） */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Utility")
	static float GetRandomAngle(float MaxAngle = 5.0f);

	/** Sin波に基づくパルス値（0-1） */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Utility")
	static float GetPulseValue(float Time, float Speed = 1.0f);

	/** 0-1の範囲でPing-Pong値を取得 */
	UFUNCTION(BlueprintPure, Category = "UI|Animation|Utility")
	static float PingPong(float Time, float Length = 1.0f);
};
