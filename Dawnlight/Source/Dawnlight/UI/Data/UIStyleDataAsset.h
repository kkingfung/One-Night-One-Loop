// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateColor.h"
#include "Fonts/SlateFontInfo.h"
#include "UIStyleDataAsset.generated.h"

/**
 * ボタンスタイル設定
 */
USTRUCT(BlueprintType)
struct FDawnlightButtonStyle
{
	GENERATED_BODY()

	/** 通常時の背景色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor NormalColor = FLinearColor(0.1f, 0.1f, 0.15f, 0.9f);

	/** ホバー時の背景色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor HoveredColor = FLinearColor(0.2f, 0.3f, 0.5f, 1.0f);

	/** 押下時の背景色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor PressedColor = FLinearColor(0.4f, 0.5f, 0.7f, 1.0f);

	/** 無効時の背景色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor DisabledColor = FLinearColor(0.05f, 0.05f, 0.05f, 0.5f);

	/** テキスト色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor TextColor = FLinearColor(0.9f, 0.9f, 0.95f, 1.0f);

	/** ホバー時のテキスト色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor TextHoveredColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	/** ボーダー色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor BorderColor = FLinearColor(0.3f, 0.4f, 0.6f, 0.8f);

	/** ホバー時のボーダー色（グロー効果用） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor BorderHoveredColor = FLinearColor(0.5f, 0.7f, 1.0f, 1.0f);

	/** ボーダー幅 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float BorderWidth = 2.0f;

	/** 角丸の半径 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ", meta = (ClampMin = "0.0", ClampMax = "50.0"))
	float CornerRadius = 8.0f;

	/** パディング */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ")
	FMargin Padding = FMargin(24.0f, 12.0f);
};

/**
 * テキストスタイル設定
 */
USTRUCT(BlueprintType)
struct FDawnlightTextStyle
{
	GENERATED_BODY()

	/** フォント */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "フォント")
	FSlateFontInfo Font;

	/** テキスト色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor Color = FLinearColor(0.9f, 0.9f, 0.95f, 1.0f);

	/** シャドウ色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor ShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.5f);

	/** シャドウオフセット */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "エフェクト")
	FVector2D ShadowOffset = FVector2D(2.0f, 2.0f);
};

/**
 * アニメーション設定
 */
USTRUCT(BlueprintType)
struct FDawnlightAnimationSettings
{
	GENERATED_BODY()

	/** フェードイン時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイミング", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float FadeInDuration = 0.3f;

	/** フェードアウト時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイミング", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float FadeOutDuration = 0.2f;

	/** ホバーアニメーション時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイミング", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HoverTransitionDuration = 0.15f;

	/** ボタン押下スケール */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スケール", meta = (ClampMin = "0.8", ClampMax = "1.0"))
	float PressedScale = 0.95f;

	/** ホバー時スケール */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "スケール", meta = (ClampMin = "1.0", ClampMax = "1.2"))
	float HoveredScale = 1.05f;

	/** メニュー項目のスタガー遅延（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "タイミング", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float StaggerDelay = 0.08f;

	/** スライドイン距離 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "移動", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float SlideInDistance = 100.0f;

	/** イージング関数タイプ (0=Linear, 6=EaseOut, 7=EaseInOut) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "イージング", meta = (ClampMin = "0", ClampMax = "25"))
	int32 EasingFunctionType = 6; // EEasingFunc::EaseOut
};

/**
 * パネルスタイル設定
 */
USTRUCT(BlueprintType)
struct FDawnlightPanelStyle
{
	GENERATED_BODY()

	/** 背景色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor BackgroundColor = FLinearColor(0.02f, 0.02f, 0.05f, 0.85f);

	/** ボーダー色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor BorderColor = FLinearColor(0.2f, 0.3f, 0.5f, 0.6f);

	/** ボーダー幅 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float BorderWidth = 1.0f;

	/** 角丸の半径 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ", meta = (ClampMin = "0.0", ClampMax = "50.0"))
	float CornerRadius = 12.0f;

	/** 内側のパディング */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ")
	FMargin Padding = FMargin(20.0f);

	/** ブラー強度（背景ぼかし） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "エフェクト", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BackgroundBlur = 10.0f;
};

/**
 * スライダースタイル設定
 */
USTRUCT(BlueprintType)
struct FDawnlightSliderStyle
{
	GENERATED_BODY()

	/** トラック背景色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor TrackColor = FLinearColor(0.1f, 0.1f, 0.15f, 0.8f);

	/** トラック塗りつぶし色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor FillColor = FLinearColor(0.3f, 0.5f, 0.8f, 1.0f);

	/** ハンドル色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor HandleColor = FLinearColor(0.9f, 0.9f, 0.95f, 1.0f);

	/** ホバー時のハンドル色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "色")
	FLinearColor HandleHoveredColor = FLinearColor(0.5f, 0.7f, 1.0f, 1.0f);

	/** トラックの高さ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ", meta = (ClampMin = "2.0", ClampMax = "20.0"))
	float TrackHeight = 6.0f;

	/** ハンドルのサイズ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サイズ", meta = (ClampMin = "8.0", ClampMax = "40.0"))
	float HandleSize = 18.0f;
};

/**
 * UIスタイルデータアセット
 *
 * Dawnlightゲーム全体のUIスタイルを定義
 * ダークで神秘的なサバイバルホラーの雰囲気を演出
 */
UCLASS(BlueprintType)
class DAWNLIGHT_API UUIStyleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// ========================================================================
	// カラーパレット
	// ========================================================================

	/** プライマリカラー（アクセント） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "カラーパレット")
	FLinearColor PrimaryColor = FLinearColor(0.3f, 0.5f, 0.8f, 1.0f);

	/** セカンダリカラー */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "カラーパレット")
	FLinearColor SecondaryColor = FLinearColor(0.5f, 0.3f, 0.6f, 1.0f);

	/** 警告色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "カラーパレット")
	FLinearColor WarningColor = FLinearColor(0.9f, 0.6f, 0.2f, 1.0f);

	/** 危険色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "カラーパレット")
	FLinearColor DangerColor = FLinearColor(0.8f, 0.2f, 0.2f, 1.0f);

	/** 成功色 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "カラーパレット")
	FLinearColor SuccessColor = FLinearColor(0.2f, 0.7f, 0.4f, 1.0f);

	/** 背景オーバーレイ色（ポーズ画面等） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "カラーパレット")
	FLinearColor OverlayColor = FLinearColor(0.0f, 0.0f, 0.02f, 0.75f);

	// ========================================================================
	// テキストスタイル
	// ========================================================================

	/** タイトル用テキストスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "テキストスタイル")
	FDawnlightTextStyle TitleStyle;

	/** 見出し用テキストスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "テキストスタイル")
	FDawnlightTextStyle HeadingStyle;

	/** 本文用テキストスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "テキストスタイル")
	FDawnlightTextStyle BodyStyle;

	/** キャプション用テキストスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "テキストスタイル")
	FDawnlightTextStyle CaptionStyle;

	// ========================================================================
	// コンポーネントスタイル
	// ========================================================================

	/** メインボタンスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンポーネントスタイル")
	FDawnlightButtonStyle PrimaryButtonStyle;

	/** セカンダリボタンスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンポーネントスタイル")
	FDawnlightButtonStyle SecondaryButtonStyle;

	/** 危険ボタンスタイル（削除、終了等） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンポーネントスタイル")
	FDawnlightButtonStyle DangerButtonStyle;

	/** パネルスタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンポーネントスタイル")
	FDawnlightPanelStyle PanelStyle;

	/** スライダースタイル */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "コンポーネントスタイル")
	FDawnlightSliderStyle SliderStyle;

	// ========================================================================
	// アニメーション設定
	// ========================================================================

	/** アニメーション設定 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "アニメーション")
	FDawnlightAnimationSettings AnimationSettings;

	// ========================================================================
	// サウンド
	// ========================================================================

	/** ボタンホバー音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TObjectPtr<USoundBase> HoverSound;

	/** ボタンクリック音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TObjectPtr<USoundBase> ClickSound;

	/** 戻る音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TObjectPtr<USoundBase> BackSound;

	/** エラー音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TObjectPtr<USoundBase> ErrorSound;

	/** 成功音 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "サウンド")
	TObjectPtr<USoundBase> SuccessSound;
};
