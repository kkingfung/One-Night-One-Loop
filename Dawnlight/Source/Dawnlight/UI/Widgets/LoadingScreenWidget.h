// Soul Reaper - Dawnlight Project
// Copyright (c) 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DawnlightWidgetBase.h"
#include "LoadingScreenWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

/**
 * ローディング画面ウィジェット
 *
 * レベル遷移時に表示されるローディング画面
 * - プログレスバー
 * - ローディングテキスト
 * - ヒントテキスト（オプション）
 * - フェードイン/アウトアニメーション
 */
UCLASS()
class DAWNLIGHT_API ULoadingScreenWidget : public UDawnlightWidgetBase
{
	GENERATED_BODY()

public:
	ULoadingScreenWidget(const FObjectInitializer& ObjectInitializer);

	// ========================================================================
	// 進捗管理
	// ========================================================================

	/** ローディング進捗を設定 (0.0 - 1.0) */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void SetProgress(float Progress);

	/** 現在の進捗を取得 */
	UFUNCTION(BlueprintPure, Category = "ローディング")
	float GetProgress() const { return CurrentProgress; }

	/** ローディングテキストを設定 */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void SetLoadingText(const FText& Text);

	/** ヒントテキストを設定 */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void SetHintText(const FText& Text);

	// ========================================================================
	// 表示制御
	// ========================================================================

	/** フェードインで表示開始 */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void ShowWithFadeIn();

	/** フェードアウトで非表示 */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void HideWithFadeOut();

	/** 即座に表示 */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void ShowImmediately();

	/** 即座に非表示 */
	UFUNCTION(BlueprintCallable, Category = "ローディング")
	void HideImmediately();

	// ========================================================================
	// ヒントシステム
	// ========================================================================

	/** ランダムなヒントを表示 */
	UFUNCTION(BlueprintCallable, Category = "ローディング|ヒント")
	void ShowRandomHint();

	/** 次のヒントに切り替え */
	UFUNCTION(BlueprintCallable, Category = "ローディング|ヒント")
	void ShowNextHint();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========================================================================
	// UI要素（BP側でバインド）
	// ========================================================================

	/** プログレスバー */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	/** ローディングテキスト */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LoadingText;

	/** ヒントテキスト */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HintText;

	/** 背景画像 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> BackgroundImage;

	/** ローディングアイコン（回転アニメーション用） */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> LoadingIcon;

	// ========================================================================
	// 設定
	// ========================================================================

	/** ローディングヒント一覧 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ローディング|設定")
	TArray<FText> LoadingHints;

	/** ヒント切り替え間隔（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ローディング|設定", meta = (ClampMin = "2.0", ClampMax = "10.0"))
	float HintChangeInterval = 5.0f;

	/** プログレスバーのスムージング速度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ローディング|設定", meta = (ClampMin = "1.0", ClampMax = "20.0"))
	float ProgressSmoothSpeed = 5.0f;

	/** ローディングアイコンの回転速度（度/秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ローディング|設定")
	float IconRotationSpeed = 180.0f;

	/** フェード時間（秒） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ローディング|設定", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float FadeDuration = 0.5f;

private:
	/** 現在の進捗（実際の値） */
	float CurrentProgress;

	/** 表示用の進捗（スムージング適用後） */
	float DisplayProgress;

	/** 現在のヒントインデックス */
	int32 CurrentHintIndex;

	/** ヒント表示タイマー */
	float HintTimer;

	/** フェード中フラグ */
	bool bIsFading;

	/** フェードイン中か（falseならフェードアウト） */
	bool bIsFadingIn;

	/** フェード進捗 */
	float FadeProgress;

	/** プログレスバーを更新 */
	void UpdateProgressBar(float DeltaTime);

	/** ローディングアイコンを回転 */
	void UpdateLoadingIcon(float DeltaTime);

	/** ヒントタイマーを更新 */
	void UpdateHintTimer(float DeltaTime);

	/** フェードを更新 */
	void UpdateFade(float DeltaTime);

	/** デフォルトのヒントを設定 */
	void SetupDefaultHints();
};
