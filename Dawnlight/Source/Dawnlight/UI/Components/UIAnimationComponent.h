// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIAnimationComponent.generated.h"

class UWidget;
class UUserWidget;
class UUIStyleDataAsset;

/**
 * UIアニメーションの種類
 */
UENUM(BlueprintType)
enum class EUIAnimationType : uint8
{
	None			UMETA(DisplayName = "なし"),
	FadeIn			UMETA(DisplayName = "フェードイン"),
	FadeOut			UMETA(DisplayName = "フェードアウト"),
	SlideInLeft		UMETA(DisplayName = "左からスライドイン"),
	SlideInRight	UMETA(DisplayName = "右からスライドイン"),
	SlideInTop		UMETA(DisplayName = "上からスライドイン"),
	SlideInBottom	UMETA(DisplayName = "下からスライドイン"),
	ScaleIn			UMETA(DisplayName = "スケールイン"),
	ScaleOut		UMETA(DisplayName = "スケールアウト"),
	Bounce			UMETA(DisplayName = "バウンス"),
	Pulse			UMETA(DisplayName = "パルス")
};

/**
 * UIアニメーションコンポーネント
 *
 * ウィジェットにジューシーなアニメーションを追加するためのコンポーネント
 * - フェードイン/アウト
 * - スライドイン
 * - スケールアニメーション
 * - スタガーアニメーション（複数要素の連続アニメーション）
 */
UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class DAWNLIGHT_API UUIAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIAnimationComponent();

	// ========================================================================
	// 静的ヘルパー関数（ウィジェット内から直接呼び出し用）
	// ========================================================================

	/** ウィジェットをフェードイン */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayFadeIn(UWidget* Widget, float Duration = 0.3f, float Delay = 0.0f);

	/** ウィジェットをフェードアウト */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayFadeOut(UWidget* Widget, float Duration = 0.2f, float Delay = 0.0f);

	/** ウィジェットをスライドイン */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlaySlideIn(UWidget* Widget, EUIAnimationType Direction, float Distance = 100.0f, float Duration = 0.3f, float Delay = 0.0f);

	/** ウィジェットをスケールアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayScaleAnimation(UWidget* Widget, float StartScale, float EndScale, float Duration = 0.2f, float Delay = 0.0f);

	/** ボタンホバーアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayButtonHover(UWidget* Widget, bool bIsHovered, float Scale = 1.05f, float Duration = 0.15f);

	/** ボタン押下アニメーション */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayButtonPress(UWidget* Widget, float Scale = 0.95f, float Duration = 0.1f);

	/** 複数ウィジェットをスタガーアニメーション */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayStaggeredAnimation(const TArray<UWidget*>& Widgets, EUIAnimationType AnimationType, float StaggerDelay = 0.08f, float Duration = 0.3f);

	/** パルスアニメーション（注意を引く） */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayPulse(UWidget* Widget, float MinScale = 0.95f, float MaxScale = 1.05f, float Duration = 0.5f, bool bLoop = true);

	/** シェイクアニメーション（エラー時等） */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayShake(UWidget* Widget, float Intensity = 10.0f, float Duration = 0.3f);

	/** グローエフェクト（ボーダーの輝き） */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void PlayGlow(UWidget* Widget, FLinearColor GlowColor, float Duration = 0.3f);

	// ========================================================================
	// ユーティリティ
	// ========================================================================

	/** 全てのアニメーションを停止 */
	UFUNCTION(BlueprintCallable, Category = "UIアニメーション")
	static void StopAllAnimations(UWidget* Widget);

	/** アニメーションが再生中か確認 */
	UFUNCTION(BlueprintPure, Category = "UIアニメーション")
	static bool IsAnimating(UWidget* Widget);
};
